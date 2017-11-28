#include "CancleBankerProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include <json/json.h>
#include "ProcessManager.h"

CancleBankerProc::CancleBankerProc()
{
	this->name = "CancleBankerProc";
}

CancleBankerProc::~CancleBankerProc()
{

} 

int CancleBankerProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType() ;
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short realTid = tid & 0x0000FFFF;
	short sid = pPacket->ReadShort();

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

    Table *table = Room::getInstance()->getTable(realTid);
	int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("CancleBankerProc:table is NULL\n");
		sendErrorMsg(clientHandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("CancleBankerProc: uid[%d] Player is NULL\n", uid);
		sendErrorMsg(clientHandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		return 0;
	}

    // 做庄的只能在闲状态下庄
	if(table->bankeruid == player->id)
	{
        int player_cnt = table->getPlayerCount(true, true);
        ULOGGER(E_LOG_INFO, uid) << "request cancle banker, bankerid = " << table->bankeruid << ", table_status = " << table->m_nStatus << ", player_cnt = " << player_cnt;
        if (table->m_nStatus != STATUS_TABLE_IDLE && player_cnt > 0)
        {
            // 延迟到下个空闲状态的时候处理
            player->isCanlcebanker = true;
            return sendErrorMsg(clientHandler, cmd, uid, -28, ERRMSG(-28), seq);
        }

		OutputPacket response;
		response.Begin(cmd, player->id);
		response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(player->id);
		response.WriteShort(player->m_nStatus);
		response.WriteInt(table->id);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(player->id);
		response.End();
        if (HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
        {
            ULOGGER(E_LOG_ERROR, uid) << "Send To cancle banker failed!";
        }
        else
        {
            ULOGGER(E_LOG_ERROR, uid) << "Send To cancle banker success!";
            table->rotateBanker();
            table->GameStart();
        }

		return 0;
	}

    // 等待上庄的随时可以去消申请
    if (!table->isWaitForBanker(uid))
    {
        _LOG_ERROR_("Your uid[%d] Not In BankerList sid[%d]\n", uid, sid);
        return sendErrorMsg(clientHandler, cmd, uid, -10, ErrorMsg::getInstance()->getErrMsg(-10), seq);
    }

	player->setActiveTime(time(NULL));
	table->CancleBanker(player);

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, player, seq);
		}
	}

	return 0;
}


int CancleBankerProc::sendTabePlayersInfo(Player* player, Table* table, Player* applyer, short seq)
{
	OutputPacket response;
	response.Begin(CLIENT_MSG_CANCLEBANKER, player->id);
	if(player->id == applyer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(applyer->id);
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[CancleBankerProc] Send To Uid[%d] Error!\n", player->id);
	else
		_LOG_DEBUG_("[CancleBankerProc] Send To Uid[%d] Success\n", player->id);
	return 0;
}


REGISTER_PROCESS(CLIENT_MSG_CANCLEBANKER, CancleBankerProc)
