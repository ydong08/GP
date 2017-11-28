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
	short sid = pPacket->ReadShort();

	_LOG_INFO_("==>[GetHistoryProc] [0x%04x] uid[%d] tid[%d]\n", cmd, uid, tid);
	_LOG_DEBUG_("[DATA Parse] sid=[%d]\n", sid);

	CDLSocketHandler* hallhandler = (clientHandler);

	Room* room = Room::getInstance();
    Table *table = room->getTable();
	if(table==NULL)
	{
		_LOG_ERROR_("GetHistoryProc: uid[%d] table is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}
	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("GetHistoryProc: uid[%d] Player is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-1),seq);
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
	int num = 0;
	if(table->m_GameRecordArrary[table->m_nRecordLast].cbBanker == 0)
	{
		response.WriteShort(table->m_nRecordLast);
		num = table->m_nRecordLast;
		for(int i = 0; i < table->m_nRecordLast; ++i)
		{
			response.WriteByte(table->m_GameRecordArrary[i].cbBanker);
			response.WriteByte(table->m_GameRecordArrary[i].cbTian);
			response.WriteByte(table->m_GameRecordArrary[i].cbDi);
			response.WriteByte(table->m_GameRecordArrary[i].cbXuan);
			response.WriteByte(table->m_GameRecordArrary[i].cbHuang);
		}
	}
	else
	{
		response.WriteShort(MAX_SCORE_HISTORY);
		num = MAX_SCORE_HISTORY;
		for (int i = 0; i < MAX_SCORE_HISTORY; ++i)
		{
			int index = (table->m_nRecordLast+i)%MAX_SCORE_HISTORY;
			response.WriteByte(table->m_GameRecordArrary[index].cbBanker);
			response.WriteByte(table->m_GameRecordArrary[index].cbTian);
			response.WriteByte(table->m_GameRecordArrary[index].cbDi);
			response.WriteByte(table->m_GameRecordArrary[index].cbXuan);
			response.WriteByte(table->m_GameRecordArrary[index].cbHuang);
		}
	}
	response.End();

	_LOG_DEBUG_("<==[GetHistoryProc] [0x%04x]\n", cmd);
	_LOG_DEBUG_("[Data Response] retcode=[%d] retmsg=[%s]\n", 0, "ok");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] num=[%d]\n", num);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetHistoryProc] Send To Uid[%d] Error!\n", player->id);
	//else
		//_LOG_DEBUG_("[GetHistoryProc] Send To Uid[%d] Success\n", player->id);
	
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_GETHISTORY, GetHistoryProc)
