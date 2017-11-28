#include <string>
#include "StartGameProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameUtil.h"
#include "AllocSvrConnect.h"
#include "ProcessManager.h"
#include "IProcess.h"
#include "BaseClientHandler.h"
#include "ProtocolServerId.h"

REGISTER_PROCESS(CLIENT_MSG_START_GAME, StartGameProc)

StartGameProc::StartGameProc()
{
	this->name = "StartGameProc" ;
}

StartGameProc::~StartGameProc()
{
}

int StartGameProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_DEBUG_("==>[StartGameProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] uid=[%d] \n", uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[StartGameProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,_EMSG_(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[StartGameProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,_EMSG_(-1),seq);
	}

	if(player->isActive())
		return sendErrorMsg(hallhandler, cmd, uid, -8,_EMSG_(-8),seq);

	if(player->isReady())
		return sendErrorMsg(hallhandler, cmd, uid, -9,_EMSG_(-9),seq);

	
	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] m_lMoney is too small m_lMoney[%d]\n", uid, player->m_lMoney);
		sendErrorMsg(hallhandler, cmd, uid, -4,_EMSG_(-4),seq);
		return 0;
	}

	_LOG_INFO_("[StartGameProc] UID=[%d] tid=[%d] realTid=[%d] StartGame OK\n", uid, tid, realTid);
	
	player->m_nStatus = STATUS_PLAYER_RSTART;
	player->setActiveTime(time(NULL));
	player->setRePlayeTime(time(NULL));

	int sendnum = 0;
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, table->m_nCountPlayer, player,seq);
			sendnum++;
		}
	}

/*	if (table->m_nCountPlayer < GAME_PLAYER)
	{
		std::string		strTrumpt;

		if (GameUtil::getInvitePlayerTrumpt(strTrumpt, Configure::getInstance()->m_nGameID, Configure::getInstance()->m_nLevel, realTid, table->m_nCountPlayer, GAME_PLAYER))
			AllocSvrConnect::getInstance()->trumptToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), player->pid);
	}*/

	if(table->isAllReady() && table->m_nCountPlayer > 1)
		return IProcess::GameStart(table);

	if(!table->isLock() && table->m_nType != PRIVATE_ROOM)
		//设置踢出时间
		table->setKickTimer();
	
	if(table->isActive())
		return 0;

	if (table->isCanGameStart())
	{
		table->startTableStartTimer(Configure::getInstance()->tablestarttime);
	}


	return 0;
}

int StartGameProc::sendTabePlayersInfo(Player* player, Table* table, short num, Player* starter, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_START_GAME, player->id);
	if(player->id == starter->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(starter->id);
	response.WriteShort(starter->m_nStatus);
	response.End();
	_LOG_DEBUG_("<==[StartGameProc] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_START_GAME, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] startuid=[%d]\n", starter->id);
	_LOG_DEBUG_("[Data Response] starter m_nStatus=[%d]\n", starter->m_nStatus);

	

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[StartGameProc] Send To Uid[%d] Error!\n", player->id);
	
	return 0;
}

int StartGameProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}



