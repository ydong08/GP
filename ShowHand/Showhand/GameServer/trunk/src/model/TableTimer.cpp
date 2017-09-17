#include "TableTimer.h"
#include "IProcess.h"
#include "Table.h"
#include "Logger.h"
#include "Configure.h"
#include "HallManager.h"
#include "UdpManager.h"
#include "LogServerConnect.h"
//====================CTimer_Handler==================================//

int CTimer_Handler::ProcessOnTimerOut()
{
	if(handler)
		return handler->ProcessOnTimerOut(this->timeid, this->uid);
	else
		return 0;

}

void CTimer_Handler::SetTimeEventObj(TableTimer * obj, int timeid, int uid)
{
	this->handler = obj;
	this->timeid = timeid;
	this->uid = uid;
}


//==========================TableTimer==================================//

void TableTimer::init(Table* table)
{
	this->table = table;
}

void TableTimer::stopAllTimer()
{
	stopBetCoinTimer();
	stopKickTimer();
	stopTableStartTimer();
}

void TableTimer::startBetCoinTimer(int uid,int timeout)
{
	m_BetTimer.SetTimeEventObj(this, BET_COIN_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void TableTimer::stopBetCoinTimer()
{
	m_BetTimer.StopTimer();
}

void TableTimer::startTableStartTimer(int timeout)
{
	m_TableStartTimer.SetTimeEventObj(this, TABLE_START_TIMER);
	m_TableStartTimer.StartTimer(timeout);
}

void TableTimer::stopTableStartTimer()
{
	m_TableStartTimer.StopTimer();
}

void TableTimer::startKickTimer(int timeout)
{
	m_TableKickTimer.SetTimeEventObj(this, TABLE_KICK_TIMER);
	m_TableKickTimer.StartTimer(timeout);
}

void TableTimer::stopKickTimer()
{
	m_TableKickTimer.StopTimer();
}

void TableTimer::startSendCardTimer(int timeout)
{
	m_TableKickTimer.SetTimeEventObj(this, TABLE_SEND_TIMER);
	m_TableKickTimer.StartMinTimer(timeout);
}

void TableTimer::stopSendCardTimer()
{
	m_TableKickTimer.StopTimer();
}

int TableTimer::ProcessOnTimerOut(int Timerid, int uid)
{
	switch (Timerid)
	{
	case BET_COIN_TIMER:
		return BetTimeOut(uid);
	case TABLE_START_TIMER:
		return TableGameStartTimeOut();
	case TABLE_KICK_TIMER:
		return TableKickTimeOut();
	case TABLE_SEND_TIMER:
		return SendCardTimeOut();
	default:
		return 0;
	}
	return 0;
}

int TableTimer::BetTimeOut(int uid)
{
	this->stopBetCoinTimer();
	Player* player = table->getPlayer(uid);
	
	if(player == NULL)
	{
		_LOG_ERROR_("[BetTimeOut] uid=[%d] tid=[%d] Your not in This Table\n",uid, this->table->id);
		return 0;
	}

	if(uid != table->betUid)
	{
		_LOG_ERROR_("[BetTimeOut] uid=[%d] tid=[%d] current betuid[%d]\n",uid, this->table->id, table->betUid);
		return 0;
	}

	_LOG_INFO_("BetTimeOut tid=[%d] uid=[%d]\n", this->table->id, uid);

	//如果是第一轮用户就下注超时则记录下来，方便后续踢出用户
	if(table->currRound == 1)
		player->timeoutCount++;
	Player* nextplayer = NULL;
	if(this->table->currRound != 5)
	{
		//表示此人已经弃牌
		player->hascard = false;
		//设置下一个应该下注的用户
		nextplayer = table->getNextBetPlayer(player,OP_THROW);

	}
	else
	{
		//设置当最后一轮默认帮用户开牌
		nextplayer = table->getNextBetPlayer(player,OP_CHECK);
	}

	if(nextplayer)
	{
		table->setPlayerlimitcoin(nextplayer);
	}

	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["Time"]=(int)(time(NULL) - table->getStartTime());
	data["currd"] = table->currRound;
	data["timeoutID"] = player->id;
	data["count"] = (double)player->betCoinList[0];
	if(!table->isAllRobot())
		_LOG_REPORT_(player->id, RECORD_TIME_OUT, "%s", data.toStyledString().c_str());
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, GMSERVER_BET_TIMEOUT,"%s",data.toStyledString().c_str());

	int sendNum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendNum == table->countPlayer)
			break;
		if(table->player_array[i])
		{
			if(table->currRound != 5)
				sendBetTimeOut(table->player_array[i], table, player, nextplayer);
			else
				sendOpenTimeOut(table->player_array[i], table, player, nextplayer);
			sendNum++;
		}
	}

	Player* winner = NULL;
	if(table->iscanGameOver(&winner))
		return IProcess::GameOver(table, winner, true);

	//当前已经没有下一个用户下注了此轮结束
	if(nextplayer == NULL)
	{
		table->setNextRound();
	}
	else
	{
		table->startBetCoinTimer(nextplayer->id,Configure::getInstance()->betcointime);
		nextplayer->setBetCoinTime(time(NULL));
	}
	return 0;
}

int TableTimer::sendBetTimeOut(Player* player, Table* table, Player* timeoutplayer,Player* nextplayer)
{
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(GMSERVER_BET_TIMEOUT, player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteShort(table->currRound);
	response.WriteInt(timeoutplayer->id);
	int64_t nextlimitcoin = -1;
	if(nextplayer)
	{
		nextlimitcoin = nextplayer->nextlimitcoin;
		response.WriteInt(nextplayer->id);
		response.WriteShort(nextplayer->optype);
		response.WriteInt64(nextlimitcoin);
		response.WriteInt64(table->currMaxCoin - nextplayer->betCoinList[table->currRound]);
	}
	else
	{
		response.WriteInt(0);
		response.WriteShort(0);
		response.WriteInt64(-1);
		response.WriteInt64(-1);
	}
	response.End();
	_LOG_INFO_("<==[sendBetTimeOut] Push [0x%04x] to uid=[%d]\n", GMSERVER_BET_TIMEOUT, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] status=[%d]\n",player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] currRound=[%d]\n", table->currRound);
	_LOG_DEBUG_("[Data Response] timeoutplayer_id=[%d]\n",timeoutplayer->id);
	_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", nextplayer ? nextplayer->optype : 0);
	_LOG_DEBUG_("[Data Response] nextlimitcoin=[%ld]\n", nextlimitcoin);
	_LOG_DEBUG_("[Data Response] differcoin=[%ld]\n",nextplayer ? table->currMaxCoin - nextplayer->betCoinList[table->currRound] : -1);
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[BetCallProc] Send To Uid[%d] Error!\n", player->id);
	
	return 0;
}

int TableTimer::sendOpenTimeOut(Player* player, Table* table, Player* timeoutplayer,Player* nextplayer)
{
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_LOOK_CARD, player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteShort(table->currRound);
	response.WriteInt(timeoutplayer->id);
	int64_t nextlimitcoin = -1;
	if(nextplayer)
	{
		nextlimitcoin = nextplayer->nextlimitcoin;
		response.WriteInt(nextplayer->id);
		response.WriteShort(nextplayer->optype);
		response.WriteInt64(nextlimitcoin);
		response.WriteInt64(table->currMaxCoin - nextplayer->betCoinList[table->currRound]);
	}
	else
	{
		response.WriteInt(0);
		response.WriteShort(0);
		response.WriteInt64(-1);
		response.WriteInt64(-1);
	}
	response.End();
	_LOG_INFO_("<==[sendOpenTimeOut] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_LOOK_CARD, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] status=[%d]\n",player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] currRound=[%d]\n", table->currRound);
	_LOG_DEBUG_("[Data Response] timeoutplayer_id=[%d]\n",timeoutplayer->id);
	_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", nextplayer ? nextplayer->optype : 0);
	_LOG_DEBUG_("[Data Response] nextlimitcoin=[%ld]\n", nextlimitcoin);
	_LOG_DEBUG_("[Data Response] differcoin=[%ld]\n",nextplayer ? table->currMaxCoin - nextplayer->betCoinList[table->currRound] : -1);
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[BetCallProc] Send To Uid[%d] Error!\n", player->id);
	
	return 0;
}

int TableTimer::TableGameStartTimeOut()
{
	this->stopTableStartTimer();
	_LOG_INFO_("TableGameStartTimeOut tid=[%d]\n", this->table->id);
	if(table->isActive())
	{
		_LOG_WARN_("this table[%d] is Active\n",table->id);
		return 0;
	}
	int i;
	if (table->isCanGameStart())
	{
		for(i = 0; i < GAME_PLAYER; ++i)
		{
			Player* getplayer = table->player_array[i];
			//把没有准备的用户踢出
			if(getplayer && (getplayer->isComming() || getplayer->isGameOver()))
			{
				IProcess::serverPushLeaveInfo(table, getplayer);
				table->playerLeave(getplayer);
			}
		}
		if(table->isAllReady()&&table->countPlayer > 1)
			return IProcess::GameStart(table);
		else
			_LOG_ERROR_("table[%d] countPlayer[%d] not all ready\n", table->id, table->countPlayer);
	}
	else
		_LOG_WARN_("table[%d] is Can't GameStart PlayerCount[%d]\n", table->id, table->countPlayer);
	return 0;
}

int TableTimer::TableKickTimeOut()
{
	this->stopTableStartTimer();
	_LOG_INFO_("TableKickTimeOut tid=[%d]\n", this->table->id);
	table->unlockTable();
	if(table->isActive())
	{
		_LOG_WARN_("this table[%d] is Active\n",table->id);
		return 0;
	}

	if(table->countPlayer == 1)
	{
		_LOG_WARN_("this table[%d] is One Player\n",table->id);
		return 0;
	}
	int i;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* getplayer = table->player_array[i];
		//把没有准备的用户踢出
		if(getplayer && !getplayer->isReady())
		{
			IProcess::serverPushLeaveInfo(table, getplayer, 2);
			table->playerLeave(getplayer);
		}
	}

	if(table->isAllReady()&&table->countPlayer > 1)
		return IProcess::GameStart(table);
	else
		_LOG_ERROR_("table[%d] countPlayer[%d] not all ready\n", table->id, table->countPlayer);
	return 0;
}

int TableTimer::SendCardTimeOut()
{
	this->stopSendCardTimer();
	_LOG_INFO_("SendCardTimeOut tid=[%d]\n", this->table->id);
	table->setNextRound();
	return 0;
}


