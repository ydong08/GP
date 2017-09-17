#include <json/json.h>
#include <string>
#include <algorithm>
#include "CreateRoomProc.h"
#include "ProcessManager.h"
#include "BaseClientHandler.h"
#include "ErrorMsg.h"
#include "Logger.h"
#include "GameCmd.h"
#include "Room.h"
#include "Table.h"
#include "HallManager.h"
#include "AllocSvrConnect.h"
#include "CoinConf.h"
#include "Protocol.h"
#include "RedisLogic.h"
#include "GameApp.h"
#include "Utility.h"
#include "GameUtil.h"
#include "RoomCardUtil.h"

REGISTER_PROCESS(CLIENT_MSG_CREATE_ROOM, CreateRoomProc)

CreateRoomProc::CreateRoomProc()
{
	this->name = "CreateRoomProc";
}

CreateRoomProc::~CreateRoomProc()
{
}

int CreateRoomProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
    CreateRoomParams* pParams = (CreateRoomParams*)malloc(sizeof(CreateRoomParams));
    pt->data = pParams;
    pParams->m_Command = inputPacket->GetCmdType();
	pParams->m_Sequence = inputPacket->GetSeqNum();
	pParams->m_GameID = inputPacket->GetOptType();
	pParams->m_Source = inputPacket->GetSource();
	// US返回码
	pParams->m_ReturnCode = inputPacket->ReadShort();
	// US返回信息
    std::string retMsg = inputPacket->ReadString();
    strncpy(pParams->m_ReturnMsg, retMsg.c_str(), 256);
	// 桌子ID
	int packedTid = inputPacket->ReadInt();
	pParams->m_TableID = GameUtil::decodeTableId(packedTid);
	// 加入类型
	pParams->m_JoinType = inputPacket->ReadByte();
	// 游戏等级
	pParams->m_GameLevel = inputPacket->ReadShort();
	// 玩家ID
	pParams->m_PlayerID = inputPacket->ReadInt();
	// 参数
    std::string jsonOptions = inputPacket->ReadString();
    strncpy(pParams->m_JsonOptions, jsonOptions.c_str(), 512);
	// 大厅ID (hall服务器拼接的)
	pParams->m_HallID = inputPacket->ReadInt();

	ULOGGER(E_LOG_DEBUG, pParams->m_PlayerID) << "m_Command = " << TOHEX(pParams->m_Command)
		<< " pParams->m_Sequence = " << pParams->m_Sequence
		<< " pParams->m_GameID = " << pParams->m_GameID
		<< " pParams->m_Source = " << pParams->m_Source
		<< " pParams->m_ReturnCode = " << pParams->m_ReturnCode
		<< " pParams->m_ReturnMsg = " << pParams->m_ReturnMsg
		<< " packedTid = " << packedTid
		<< " pParams->m_TableID = " << pParams->m_TableID
		<< " pParams->m_JoinType = " << pParams->m_JoinType
		<< " pParams->m_GameLevel = " << pParams->m_GameLevel
		<< " pParams->m_PlayerID = " << pParams->m_PlayerID
		<< " pParams->m_JsonOptions = " << pParams->m_JsonOptions
		<< " pParams->m_HallID = " << pParams->m_HallID;

    //进入房间，直接处理
    if (pParams->m_JoinType == 1) {
        params = pParams;
        return process();
    }

    //创建房间，需要查询房卡数量
    MoneyAgent* connect = MoneyServer();
    OutputPacket response;
    response.Begin(pParams->m_Command, pParams->m_PlayerID);
    response.SetSeqNum(pt->seq);
    response.WriteInt(pParams->m_PlayerID);
    response.WriteShort(Configure::getInstance()->m_nServerId);
    response.End();

    if (connect->Send(&response) >= 0)
    {
        _LOG_DEBUG_("Transfer request to Back_MoneyServer OK\n");
        return 1;
    }
    else
    {
        sendErrorMsgInternal(-1, "连接服务器失败");
        _LOG_ERROR_("==>[CreateRoomProc] [0x%04x] uid=[%d] ERROR:[%s]\n", cmd, pParams->m_PlayerID, "Send request to MoneyServer Error");
        return -1;
    }
}

int CreateRoomProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(clientHandler);
    if(pt==NULL || pt->client == NULL)
    {
        _LOG_ERROR_("[CreateRoomProc:doResponse]Context is NULL\n");
        return -1;
    }
    params = (struct CreateRoomParams*)pt->data;

    short retno = inputPacket->ReadShort();
    string retmsg = inputPacket->ReadString();
    LOGGER(E_LOG_DEBUG) << "retno:" << retno;
    if (retno != 0)
    {
        _LOG_ERROR_("[%s:%s:%d]Select User Score Info Error[%d]:[%s]\n", __FILE__, __FUNCTION__, __LINE__, retno, retmsg.c_str());
        sendErrorMsgInternal(-1, (char*)retmsg.c_str());
        return -1;
    }
    int uid = inputPacket->ReadInt();
    int64_t money = inputPacket->ReadInt64();
    int64_t safemoney = inputPacket->ReadInt64();
    int32_t roll = inputPacket->ReadInt();
    int32_t roll1 = inputPacket->ReadInt();
    params->m_lRoomCard = inputPacket->ReadInt();
	int32_t exp = inputPacket->ReadInt();

	ULOGGER(E_LOG_DEBUG, uid) << "money:" << money << " roomcard:" << params->m_lRoomCard;

    return process();
}

int CreateRoomProc::parseDynamicInfo(std::string &jsonOptions, int &actualMatchCount, bool &isCharged, RoomCardMember& rcmem) {
	Json::Reader reader;
	Json::Value rootNode;
	if (jsonOptions.size() > 2 && jsonOptions[0] == '{' && reader.parse(jsonOptions, rootNode))
	{
		actualMatchCount = Utility::ParseJsonInt(rootNode, "actual_match_count", 0);
		isCharged = (Utility::ParseJsonInt(rootNode, "is_charged", 0) != 0 ) ? true : false;
		if (isCharged) {
			Json::Value member = rootNode["player_member"];
			for (int i = 0; i < (int)member.size(); i++) {
				IPlayerInfo pi;
				pi.uid = Utility::ParseJsonInt(member[i], "uid", 0);
				pi.index = Utility::ParseJsonInt(member[i], "index", 0);
				pi.max_wins = Utility::ParseJsonInt(member[i], "max_wins", 0);
				pi.win_times = Utility::ParseJsonInt(member[i], "win_times", 0);
				pi.start_money = Utility::ParseJsonInt(member[i], "start_money", 0);
                LOGGER(E_LOG_DEBUG) << "Member:" << pi.uid << " max_wins:" << pi.max_wins << " start_money:" << pi.start_money;
				rcmem[pi.uid] = pi;
			}
		}
	}
	return 0;
}

void CreateRoomProc::packCommonFields(OutputPacket& response, short retCode, const char *retMsg, Table *table)
{
	assert(table != NULL);
	
	// 操作结果
	response.WriteShort(retCode);
	// 提示信息
	response.WriteString(retMsg);
	// 加入方式
	response.WriteByte(params->m_JoinType);
	// 游戏类型
	response.WriteShort(params->m_GameLevel);
	// 玩家ID
	response.WriteInt(params->m_PlayerID);
	// 房间号 (桌号)
	int tid = table->getId();
	response.WriteInt(GameUtil::encodeTableId(tid));
	response.WriteInt(tid);
}

string CreateRoomProc::packOptionFields(Table *pTable) {
	return pTable->getTableOptions(params->m_JoinType);
}

int CreateRoomProc::sendNormalMsgInternal(short retCode, const char *retMsg, Table *table)
{
	if (table == NULL)
	{
		ULOGGER(E_LOG_ERROR, params->m_PlayerID) << "Point of table is invalid!";
		return -1;
	}

	OutputPacket response;
	response.Begin(params->m_Command, params->m_PlayerID);
	{
		response.SetSeqNum(params->m_Sequence);
		packCommonFields(response, retCode, retMsg, table);
		response.WriteString(packOptionFields(table));
	}
	response.End();

	if (HallManager::getInstance()->sendToHall(params->m_HallID, &response, false) < 0)
	{
		ULOGGER(E_LOG_ERROR, params->m_PlayerID) << "Send create result to player failed!";
		return -1;
	}
	else
	{
		ULOGGER(E_LOG_INFO, params->m_PlayerID) << "Send create result to player success!";
		return 1;
	}
}

int CreateRoomProc::sendErrorMsgInternal(short errcode, const char * errMsg)
{
	OutputPacket response;
	response.Begin(params->m_Command, params->m_PlayerID);
	response.SetSeqNum(params->m_Sequence);
	response.WriteShort(errcode);
	if (errMsg == NULL)
	{
		response.WriteString(ERRMSG(errcode));
	}
	else
	{
		response.WriteString(errMsg);
	}
	response.End();

	return HallManager::getInstance()->sendToHall(params->m_HallID, &response, false);
}

Table * CreateRoomProc::getTable(int tableID)
{
	Table *pTable = Room::getInstance()->getTable(tableID);
	if (pTable != NULL)
	{
		return pTable;
	}

    int ownerUid = RoomCardUtil::getOwnerUidByTableId(params->m_GameID, tableID);
    if (ownerUid < 0)
        return NULL;

	// restore table info.
	int tmpServerID = 0, createTime;
	std::string jsonDynamicInfo = "";
	std::string jsonOptions = params->m_JsonOptions;
	if (!RedisLogic::getCardRoomInfo(
			Round(), ownerUid,
			params->m_GameID, params->m_TableID, tmpServerID, params->m_GameLevel, createTime, jsonOptions, jsonDynamicInfo))
	{
		return NULL;
	}

	int nResult = arrangeTable(&pTable, jsonOptions, jsonDynamicInfo, ownerUid, createTime, tableID);
	ULOGGER(E_LOG_ERROR, params->m_PlayerID) << "nResult = " << nResult;
	if (nResult != 0)
	{
		sendErrorMsgInternal(nResult);
	}

	return pTable;
}


int CreateRoomProc::arrangeTable(Table** ppDestTable, string jsonOptions, string gameData, int ownerId, int createTime, int tableID /*= -1*/)
{
	// 直接由大厅来安排预定桌子
	Table *destTable = Room::getInstance()->getAvailableTable();
	if (NULL == destTable)
	{
		*ppDestTable = NULL;
		// 找不到可用的桌子
		LOGGER(E_LOG_ERROR) << "Rearrange table failed!";
		return -2;
	}

	if (destTable->setTableOptions(tableID, jsonOptions) != 0) {
		return -1;
	}

	int targetMatchCount = destTable->m_nTotalRound;
	bool createMode = (tableID < 0) ? true : false;

	// tableID
	if (createMode)
	{
		int price = RoomCardUtil::getCardRoomPrice(targetMatchCount);
		// 初步验证房卡信息 (不扣除)
		if (params->m_lRoomCard < price)
		{
			// 房卡数量不足
			destTable->setId(-1);
			ULOGGER(E_LOG_ERROR, ownerId) << "Need more room passport, RoomCardPrice = " << price << ", targetMatchCount = " << targetMatchCount;
			return sendErrorMsgInternal(-34);
		}

		tableID = RoomCardUtil::randomTableID(params->m_GameID);
		destTable->setId(tableID);
	} else {
		destTable->m_nCreateTime = createTime;
	}
	// matchCount
	int actualMatchCount = 0;
	bool isCharged = false;
	parseDynamicInfo(gameData, actualMatchCount, isCharged, destTable->m_PlayerMember);

	destTable->m_nFinishRound = destTable->m_nCurrentRound = actualMatchCount;
	destTable->m_bCharged = isCharged;

	// 标记分配完成
	int nResult = destTable->setOwner(ownerId, CoinConf::getInstance()->get_room_validation(360000), jsonOptions, !createMode);
	*ppDestTable = destTable;

	return nResult;
}

int CreateRoomProc::process() {
/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 中途进入房间

	if (params->m_ReturnCode == 1)
	{
		Table *pTable = getTable(params->m_TableID);
		if (params->m_TableID <= 0 || pTable == NULL)
		{
			// tell alloc server.
			LOGGER(E_LOG_ERROR) << "table isn't exist!";
			return sendErrorMsgInternal(-2);
		}

		return sendNormalMsgInternal(params->m_ReturnCode, params->m_ReturnMsg, pTable);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 创建、首次进入房间

	Json::Reader reader;
	Json::Value rootNode;
	if (strlen(params->m_JsonOptions) < 2 || params->m_JsonOptions[0] != '{' || !reader.parse(params->m_JsonOptions, rootNode))
	{
		// 参数错误
		ULOGGER(E_LOG_ERROR, params->m_PlayerID) << "Json format error!";
		return -1;
	}

	switch (params->m_JoinType)
	{
		// 创建
		case 0:
		{
			// 1. 分配桌子
			Table *destTable = NULL;
			int nResult = arrangeTable(&destTable, params->m_JsonOptions, "", params->m_PlayerID, 0, -1);
			ULOGGER(E_LOG_ERROR, params->m_PlayerID) << "nResult = " << nResult;
			if (nResult != 0)
			{
				LOGGER(E_LOG_ERROR) << "Arrange table info failed!";
				return sendErrorMsgInternal(nResult);
			}

			// 2. 通知客户端
			return sendNormalMsgInternal(0, "", destTable);
		}
			break;

			// 进入
		case 1:
		{
			int tid = Utility::ParseJsonInt(rootNode, "table_id", 0);
			string customPasswd = Utility::ParseJsonStr(rootNode, "password", "");

			ULOGGER(E_LOG_INFO, params->m_PlayerID)
					<< " tid = " << tid
					<< " customPasswd = " << customPasswd;

			Table *pTable = getTable(tid);
			if (tid <= 0 || NULL == pTable)
			{
				// 房间号错误
                return sendErrorMsgInternal(-2);
			}

			LOGGER(E_LOG_DEBUG) << pTable->getTableOptions(1);

			if (E_MSG_SOURCE_ROBOT == params->m_Source)
			{
				if (pTable->hasThreePlayer() && pTable->isAllRobot())
				{
					// 不能全是机器人
					return sendErrorMsgInternal(-2);
				}
			}
			else
			{
				std::string& serverPasswd = pTable->m_sRoomPwd;
				if (!serverPasswd.empty() && customPasswd != serverPasswd)
				{
					// 密码错误
					return sendErrorMsgInternal(-5);
				}
			}

			pTable->updateActiveTime();
			return sendNormalMsgInternal(0, "", pTable);
		}
			break;
	}

	return 0;
}
