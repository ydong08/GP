#include "GetBankerListProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
//#include "UdpManager.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "IProcess.h"

REGISTER_PROCESS(CLIENT_MSG_GETBANKERLIST, GetBankerListProc)


GetBankerListProc::GetBankerListProc()
{
	this->name = "GetBankerListProc";
}

GetBankerListProc::~GetBankerListProc()
{

} 

int GetBankerListProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short sid = pPacket->ReadShort();

	_LOG_INFO_("==>[GetBankerListProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] sid=[%d]\n", sid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();
    Table *table = room->getTable();
	int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("GetBankerListProc: uid[%d] table is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}
	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("GetBankerListProc: uid[%d] Player is NULL\n", uid);
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

	int size = table->BankerList.size();
	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteShort(size);

	list<Player*>::iterator it;
	int num = 0;
	int sendnum = 0;
	for(it = table->BankerList.begin(); it != table->BankerList.end(); it++)
	{
		
		Player* waiter = *it;
		if(waiter)
		{
			response.WriteInt(waiter->id);
			response.WriteString(waiter->name);
			response.WriteInt64(waiter->m_lMoney);
			response.WriteShort(waiter->m_nSeatID);
			response.WriteString(waiter->json);
		}
		else
		{
			response.WriteInt(0);
			response.WriteString("");
			response.WriteInt64(0);
			response.WriteShort(0);
			response.WriteString("");
		}
	}

	response.End();

	_LOG_DEBUG_("<==[GetBankerListProc] [0x%04x]\n", cmd);
	_LOG_DEBUG_("[Data Response] retcode=[%d] retmsg=[%s]\n", 0, "ok");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] size=[%d]\n", size);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetBankerListProc] Send To Uid[%d] Error!\n", player->id);
//	else
//		_LOG_DEBUG_("[GetBankerListProc] Send To Uid[%d] Success\n", player->id);
	
	return 0;
}


int GetBankerListProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
