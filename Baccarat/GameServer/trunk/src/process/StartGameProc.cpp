#include "StartGameProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameUtil.h"
#include "AllocSvrConnect.h"
#include "GameApp.h"
#include "ProtocolServerId.h"
#include "GameCmd.h"
#include "ProcessManager.h"


REGISTER_PROCESS(CLIENT_MSG_START_GAME, StartGameProc)

StartGameProc::StartGameProc()
{
	this->name = "StartGameProc";
}

StartGameProc::~StartGameProc()
{

} 

int StartGameProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	//short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	ULOGGER(E_LOG_DEBUG, uid) << "tid= " << tid << " svid = " << svid << " realtid = " << realTid;

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		ULOGGER(E_LOG_ERROR, uid) << "Table is NULL, tid= " << tid << " svid = " << svid << " realtid = " << realTid;
		return sendErrorMsg(clientHandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		ULOGGER(E_LOG_ERROR, uid) << "Your not in This Table, tid= " << tid << " svid = " << svid << " realtid = " << realTid;
		return sendErrorMsg(clientHandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	if(player->isReady() || player->isActive())
	{
		ULOGGER(E_LOG_ERROR, uid) << "tid= " << tid << " svid = " << svid << 
			" realtid = " << realTid << " table status = " << table->m_nStatus <<
			" player status" << player->m_nStatus;
		return sendErrorMsg(clientHandler, cmd, uid, -8,ErrorMsg::getInstance()->getErrMsg(-8),seq);
	}

	short result = 0;
	/*if(!player->checkMoney(result))
	{
		_LOG_ERROR_("UID[%d] not enough money m_lMoney[%d]\n", uid, player->m_lMoney);
		if(result == 0)
		{
			sendErrorMsg(clientHandler, cmd, uid, -4, ErrorMsg::getInstance()->getErrMsg(-4), seq);
		}
		else
			sendErrorMsg(clientHandler, cmd, uid, -29,ErrorMsg::getInstance()->getErrMsg(-29),seq);
		return 0;
	}*/

	ULOGGER(E_LOG_DEBUG, uid) << "StartGame ok, tid= " << tid << " svid = " << svid << " realtid = " << realTid;

	player->m_nStatus = STATUS_PLAYER_RSTART;
	player->setActiveTime(time(NULL));
	//player->setRePlayeTime(time(NULL));
	//table->stopReadyTimer(player->id);

	int i= 0;
	short player_cnt = table->getPlayerCount(false, true);
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, player_cnt, player, seq);
		}
	}

	if (player_cnt < GAME_PLAYER)
	{
		std::string		strTrumpt;

		if (GameUtil::getInvitePlayerTrumpt(strTrumpt, GAME_ID, Configure::getInstance()->m_nLevel, realTid, player_cnt, GAME_PLAYER))
			AllocSvrConnect::getInstance()->trumptToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), player->pid);
	}

	// 有玩家并且都准备就绪就可以开始游戏
	if (table->m_nStatus == STATUS_TABLE_READY)
	{
		table->GameStart();
		return 0;
	}

	return 0;
}

int StartGameProc::sendTabePlayersInfo(Player* player, Table* table, short num, Player* starter, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = svid << 16|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_START_GAME, player->id);
	if(player->id == starter->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(starter->id);
	response.WriteShort(starter->m_nStatus);
	response.End();
	ULOGGER(E_LOG_DEBUG, player->id) << "push ok, player_id = " << player->id << " player_status= " << player->m_nStatus
		<< " tid = " << tid << " table_tstatus = " << table->m_nStatus
		<< " start_id = " << starter->id << " starter_status= " << starter->m_nStatus;
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		ULOGGER(E_LOG_ERROR, player->id) << "send to player error";
	else
		ULOGGER(E_LOG_DEBUG, player->id) << "send to player Success";
	return 0;
}

int StartGameProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
