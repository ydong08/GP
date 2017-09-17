#include "GetPlayerListProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "json/json.h"
#include "ProcessManager.h"

GetPlayerListProc::GetPlayerListProc()
{
	this->name = "GetPlayerListProc";
}

GetPlayerListProc::~GetPlayerListProc()
{

} 

int GetPlayerListProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short sid = pPacket->ReadShort();
	//当前第几页，和一页多少条数据
	short PageNo = pPacket->ReadShort();
	short PageNum = pPacket->ReadShort();

	_LOG_INFO_("==>[GetPlayerListProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] sid=[%d]\n", sid);
	_LOG_DEBUG_("[DATA Parse] PageNo=[%d]\n", PageNo);
	_LOG_DEBUG_("[DATA Parse] PageNum=[%d]\n", PageNum);

	CDLSocketHandler* hallhandler = (clientHandler);

	Room* room = Room::getInstance();
    Table *table = room->getTable();
	int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("GetPlayerListProc: uid[%d] table is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}
	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("GetPlayerListProc: uid[%d] Player is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		return 0;
	}

// 	if(player->id != uid)
// 	{
// 		_LOG_ERROR_("Your uid[%d] Not Set In this index sid[%d]\n", uid, sid);
// 		sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
// 		return 0;
// 	}

	player->setActiveTime(time(NULL));

	int total= table->PlayerList.size()-1; 

	int start = (PageNo-1)*PageNum; 
	int end= start + PageNum;         
	
	if(start<0)  start = 0;
	if(end<0) end = 0;
	else if(end > total) end = total;

	int count = end - start;

	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteShort(total);
	response.WriteShort(count);

	list<Player*>::iterator it;
	int num = 0;
	int sendnum = 0;
	for(it = table->PlayerList.begin(); it != table->PlayerList.end(); it++)
	{
		if(num >= start)
		{
			if(sendnum == count)
				break;
			Player* waiter = *it;
			if(waiter && waiter->m_nTabIndex != table->bankersid)
			{
				response.WriteInt(waiter->id);
				response.WriteString(waiter->name);
				response.WriteInt64(waiter->m_lMoney);
				response.WriteShort(waiter->m_nTabIndex);
				response.WriteString(waiter->json);
				sendnum++;
			}
		}
		num++;
	}

	response.End();

	_LOG_DEBUG_("<==[GetPlayerListProc] [0x%04x]\n", cmd);
	_LOG_DEBUG_("[Data Response] retcode=[%d] retmsg=[%s]\n", 0, "ok");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] count=[%d]\n", count);
	_LOG_DEBUG_("[Data Response] total=[%d]\n", total);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetPlayerListProc] Send To Uid[%d] Error!\n", player->id);
//	else
//		_LOG_DEBUG_("[GetPlayerListProc] Send To Uid[%d] Success\n", player->id);
	
	return 0;
}


REGISTER_PROCESS(CLIENT_MSG_GETPLAYERRLIST, GetPlayerListProc)