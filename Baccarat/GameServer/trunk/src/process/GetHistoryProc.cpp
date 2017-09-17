#include "GetHistoryProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "json/json.h"
#include "ProcessManager.h"

GetHistoryProc::GetHistoryProc()
{
	this->name = "GetHistoryProc";
}

GetHistoryProc::~GetHistoryProc()
{

} 

int GetHistoryProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short realTid = tid & 0x0000FFFF;
	short sid = pPacket->ReadShort();

	//_LOG_INFO_("==>[GetHistoryProc] [0x%04x] uid[%d]\n", cmd, uid);
	//_LOG_DEBUG_("[DATA Parse] sid=[%d]\n", sid);

	Room* room = Room::getInstance();
    Table *table = room->getTable(realTid);
	int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("GetHistoryProc: uid[%d] table is NULL\n", uid);
		sendErrorMsg(clientHandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}
	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("GetHistoryProc: uid[%d] Player is NULL\n", uid);
		sendErrorMsg(clientHandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		return 0;
	}

// 	if(player->id != uid)
// 	{
// 		_LOG_ERROR_("Your uid[%d] Not Set In this index sid[%d]\n", uid, sid);
// 		sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
// 		return 0;
// 	}

	player->setActiveTime(time(NULL));
	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	
//     for(int i = 0; i < table->m_historyCount; ++i)
//     {
//         response.WriteByte(table->m_GameRecordArrary[i].cbWinType);
//         response.WriteByte(table->m_GameRecordArrary[i].bBankerTwoPair);
//         response.WriteByte(table->m_GameRecordArrary[i].bPlayerTwoPair);
//         //response.WriteByte(table->m_GameRecordArrary[i].cbBankerCount);
//         //response.WriteByte(table->m_GameRecordArrary[i].cbPlayerCount);
//     }

	if (table->m_historyCount<10)
	{
		response.WriteShort(table->m_historyCount);
		for (int i = 0; i < table->m_historyCount; ++i)
		{
			response.WriteByte(table->m_GameRecordArrary[i].cbWinType);
			response.WriteByte(table->m_GameRecordArrary[i].bBankerTwoPair);
			response.WriteByte(table->m_GameRecordArrary[i].bPlayerTwoPair);
			//response.WriteByte(table->m_GameRecordArrary[i].cbBankerCount);
			//response.WriteByte(table->m_GameRecordArrary[i].cbPlayerCount);
		}
	}
	else
	{
		response.WriteShort(table->m_historyCount);
		for (int i = table->m_nRecordLast; i < table->m_historyCount; ++i)
		{
			response.WriteByte(table->m_GameRecordArrary[i].cbWinType);
			response.WriteByte(table->m_GameRecordArrary[i].bBankerTwoPair);
			response.WriteByte(table->m_GameRecordArrary[i].bPlayerTwoPair);
			//response.WriteByte(table->m_GameRecordArrary[i].cbBankerCount);
			//response.WriteByte(table->m_GameRecordArrary[i].cbPlayerCount);
		}
		for (int i = 0; i < table->m_nRecordLast; ++i)
		{
			response.WriteByte(table->m_GameRecordArrary[i].cbWinType);
			response.WriteByte(table->m_GameRecordArrary[i].bBankerTwoPair);
			response.WriteByte(table->m_GameRecordArrary[i].bPlayerTwoPair);
			//response.WriteByte(table->m_GameRecordArrary[i].cbBankerCount);
			//response.WriteByte(table->m_GameRecordArrary[i].cbPlayerCount);
		}

	}
	response.End();

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetHistoryProc] Send To Uid[%d] Error!\n", player->id);
	//else
		//_LOG_DEBUG_("[GetHistoryProc] Send To Uid[%d] Success\n", player->id);
	
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_GETHISTORY, GetHistoryProc)
