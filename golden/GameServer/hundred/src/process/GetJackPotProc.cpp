#include "GetJackPotProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
//#include "UdpManager.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "IProcess.h"

REGISTER_PROCESS(CLIENT_MSG_GETJACKPOT, GetJackPotProc)


GetJackPotProc::GetJackPotProc()
{
	this->name = "GetJackPotProc";
}

GetJackPotProc::~GetJackPotProc()
{

} 

int GetJackPotProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
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

	_LOG_INFO_("==>[GetJackPotProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] sid=[%d]\n", sid);
	_LOG_DEBUG_("[DATA Parse] PageNo=[%d]\n", PageNo);
	_LOG_DEBUG_("[DATA Parse] PageNum=[%d]\n", PageNum);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();
    Table *table = room->getTable();
	int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("GetJackPotProc: uid[%d] table is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}
	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("GetJackPotProc: uid[%d] Player is NULL\n", uid);
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

	int total= table->JackPotList.size(); 

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
	response.WriteShort(50);
	response.WriteShort(40);
	response.WriteShort(30);
	response.WriteInt64(table->m_lJackPot);
	response.WriteInt64(table->m_lGivePlayerJackPot);
	response.WriteInt(table->m_tRewardTime);
	response.WriteByte(table->m_bJackPotCard[0]);
	response.WriteByte(table->m_bJackPotCard[1]);
	response.WriteByte(table->m_bJackPotCard[2]);
	response.WriteByte(table->m_bJackCardType);
	response.WriteShort(total);
	response.WriteShort(count);

	list<JackPotPlayer*>::iterator it;
	int num = 0;
	int sendnum = 0;
	for(it = table->JackPotList.begin(); it != table->JackPotList.end(); it++)
	{
		if(num >= start)
		{
			if(sendnum == count)
				break;
			JackPotPlayer* waiter = *it;
			if(waiter)
			{
				response.WriteInt(waiter->id);
				response.WriteString(waiter->name);
				response.WriteString(waiter->json);
				response.WriteInt64(waiter->m_lRewardCoin);
				sendnum++;
			}
		}
		num++;
	}

	response.End();

	_LOG_DEBUG_("<==[GetJackPotProc] [0x%04x]\n", cmd);
	_LOG_DEBUG_("[Data Response] retcode=[%d] retmsg=[%s]\n", 0, "ok");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] count=[%d]\n", count);
	_LOG_DEBUG_("[Data Response] total=[%d]\n", total);
	_LOG_DEBUG_("[Data Response] m_bJackCardType=[%d]\n", table->m_bJackCardType);
	_LOG_DEBUG_("[Data Response] m_lJackPot=[%ld]\n", table->m_lJackPot);
	_LOG_DEBUG_("[Data Response] m_lGivePlayerJackPot=[%ld]\n", table->m_lGivePlayerJackPot);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetJackPotProc] Send To Uid[%d] Error!\n", player->id);
//	else
//		_LOG_DEBUG_("[GetJackPotProc] Send To Uid[%d] Success\n", player->id);
	
	return 0;
}


int GetJackPotProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
