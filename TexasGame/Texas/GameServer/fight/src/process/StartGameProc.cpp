#include <string>
#include "StartGameProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "Util.h"
#include "AllocSvrConnect.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "BaseClientHandler.h"
#include "json/json.h"
#include "IProcess.h"
#include "GameApp.h"
#include "GameUtil.h"
#include "ProtocolServerId.h"

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
	
	_LOG_INFO_("==>[StartGameProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] uid=[%d] \n", uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[StartGameProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[StartGameProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	if(player->isActive())
		return sendErrorMsg(hallhandler, cmd, uid, -8,ErrorMsg::getInstance()->getErrMsg(-8),seq);

	if(player->isReady())
		return sendErrorMsg(hallhandler, cmd, uid, -9,ErrorMsg::getInstance()->getErrMsg(-9),seq);
	//如果是初级场则再来一局不重新设置金币

	if(player->m_lCarryMoney <= player->coincfg.bigblind + player->coincfg.mustbetcoin)
	{
		player->setCarryMoney();
		if(player->m_lCarryMoney <= player->coincfg.bigblind + table->m_lTax + player->coincfg.mustbetcoin)
		{
			_LOG_ERROR_("[StartGameProc] uid=[%d] tid=[%d] realTid=[%d] Not enough m_lMoney[%d] bigblind[%d] mustbetcoin[%d]\n",
				uid, tid, realTid, player->m_lMoney, player->coincfg.bigblind, player->coincfg.mustbetcoin);
			return sendErrorMsg(hallhandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),seq);
		}
	}

	_LOG_INFO_("[StartGameProc] UID=[%d] tid=[%d] realTid=[%d] StartGame OK\n", uid, tid, realTid);

	player->m_nStatus = STATUS_PLAYER_RSTART;
	player->setActiveTime(time(NULL));
	player->setRePlayeTime(time(NULL));

	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *otherplayer = NULL;
		if (i < GAME_PLAYER)
			otherplayer = table->player_array[i];
		else {
			otherplayer = *it;
			it++;
		}
		if(otherplayer)
		{
			sendTabePlayersInfo(otherplayer, table, table->m_nCountPlayer, player,seq);
		}
	}

/*	if (table->m_nCountPlayer < GAME_PLAYER)
	{
		std::string		strTrumpt;

		if (GameUtil::getInvitePlayerTrumpt(strTrumpt, GAME_ID, Configure::getInstance()->level, realTid, table->m_nCountPlayer, GAME_PLAYER))
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
	int svid = Configure::getInstance()->server_id;
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
	response.WriteInt64(starter->m_lCarryMoney);
	response.End();
	_LOG_DEBUG_("<==[StartGameProc] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_START_GAME, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] startuid=[%d]\n", starter->id);
	_LOG_DEBUG_("[Data Response] starter m_nStatus=[%d]\n", starter->m_nStatus);
	_LOG_DEBUG_("[Data Response] starter m_lCarryMoney=[%lu]\n", starter->m_lCarryMoney);

	

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

REGISTER_PROCESS(CLIENT_MSG_START_GAME, StartGameProc)

