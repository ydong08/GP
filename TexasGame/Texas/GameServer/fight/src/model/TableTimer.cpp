#include "TableTimer.h"
#include "IProcess.h"
#include "Table.h"
#include "Logger.h"
#include "Configure.h"
#include "HallManager.h"
#include "AllocSvrConnect.h"
#include "json/json.h"
#include "GameCmd.h"
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

void TableTimer::startGameOverTimer(int timeout)
{
	m_TableOverTimer.SetTimeEventObj(this, TABLE_GAME_OVER_TIMER);
	m_TableOverTimer.StartMinTimer(timeout);
}

void TableTimer::stopGameOverTimer()
{
	m_TableOverTimer.StopTimer();
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
	case TABLE_GAME_OVER_TIMER:
		return GameOverTimeOut();
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

	if(table->m_nCurrBetUid != uid)
	{
		_LOG_ERROR_("[BetTimeOut] uid=[%d] tid=[%d] betUid[%d] is not this uid:%d\n",uid, this->table->id, table->m_nCurrBetUid, uid);
		return 0;
	}

	_LOG_INFO_("BetTimeOut GameID=[%s] tid=[%d] uid=[%d]\n", table->getGameID(), this->table->id, uid);

	//如果是第一轮用户就下注超时则记录下来，方便后续踢出用户
	if(table->m_bCurrRound == 1)
		player->timeoutCount++;

	Player* nextplayer = NULL;
	//表示此人已经弃牌
	player->m_bHasCard = false;
	//设置下一个应该下注的用户
	nextplayer = table->getNextBetPlayer(player,OP_THROW);

	if(nextplayer)
	{
		table->setPlayerlimitcoin();
		table->setPlayerOptype(player, OP_THROW);
	}
	if(table->iscanGameOver())
		nextplayer = NULL;

	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["Time"]=(int)(time(NULL) - table->getStartTime());
	data["currd"] = table->m_bCurrRound;
	data["timeoutID"] = player->id;
	data["count"] = (double)player->betCoinList[0];
	if(!table->isAllRobot())
		_LOG_REPORT_(player->id, RECORD_TIME_OUT, "%s", data.toStyledString().c_str());
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, GMSERVER_BET_TIMEOUT,"%s",data.toStyledString().c_str());

	int sendNum = 0;
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			sendBetTimeOut(table->player_array[i], table, player, nextplayer);
		}
	}

	if(table->iscanGameOver())
		return table->gameOver();

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
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt(timeoutplayer->id);
	response.WriteInt64(table->m_lCurrMax);
	response.WriteInt(nextplayer ? nextplayer->id : 0);
	response.WriteInt64(player->betCoinList[table->m_bCurrRound]);
	response.WriteInt64(player->m_lCarryMoney);
	response.WriteShort(player->optype);
	response.WriteInt64(player->m_lBetLimit);
	response.WriteInt64(table->getSumPool());
	response.End();
	_LOG_INFO_("<==[sendBetTimeOut] Push [0x%04x] to uid=[%d]\n", GMSERVER_BET_TIMEOUT, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus=[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] CarryMoney=[%d]\n", player->m_lCarryMoney);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", player->optype);
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendBetTimeOut] Send To Uid[%d] Error!\n", player->id);
	
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
		for(i = 0; i < table->m_bMaxNum; ++i)
		{
			Player* getplayer = table->player_array[i];
			//把没有准备的用户踢出
			if(getplayer && (getplayer->isComming() || getplayer->isGameOver()))
			{
				IProcess::serverPushLeaveInfo(table, getplayer);
				table->playerLeave(getplayer);
			}
		}
		if(table->isAllReady()&&table->m_nCountPlayer > 1)
			return IProcess::GameStart(table);
		else
			_LOG_ERROR_("table[%d] m_nCountPlayer[%d] not all ready\n", table->id, table->m_nCountPlayer);
	}
	else
		_LOG_WARN_("table[%d] is Can't GameStart PlayerCount[%d]\n", table->id, table->m_nCountPlayer);
	return 0;
}

int TableTimer::TableKickTimeOut()
{
	this->stopKickTimer();
	_LOG_INFO_("TableKickTimeOut tid=[%d]\n", this->table->id);
	table->unlockTable();
	if(table->isActive())
	{
		_LOG_WARN_("this table[%d] is Active\n",table->id);
		return 0;
	}

	if(table->m_nCountPlayer == 1)
	{
		_LOG_WARN_("this table[%d] is One Player\n",table->id);
		return 0;
	}
	int i;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* getplayer = table->player_array[i];
		//把没有准备的用户踢出
		if(getplayer && !getplayer->isReady())
		{
			IProcess::serverPushLeaveInfo(table, getplayer);
			table->playerLeave(getplayer);
		}
	}

	if(table->isAllReady()&&table->m_nCountPlayer > 1)
		return IProcess::GameStart(table);
	else
		_LOG_ERROR_("table[%d] m_nCountPlayer[%d] not all ready\n", table->id, table->m_nCountPlayer);
	return 0;
}

int TableTimer::SendCardTimeOut()
{
	this->stopSendCardTimer();
	_LOG_INFO_("SendCardTimeOut tid=[%d]\n", this->table->id);
	table->setNextRound();
	return 0;
}

int TableTimer::GameOverTimeOut()
{
	this->stopGameOverTimer();
	_LOG_INFO_("GameOverTimeOut tid=[%d]\n", this->table->id);
	for(int i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* player = table->player_array[i];
		if(player && player->isActive())
		{
			player->m_nStatus = STATUS_PLAYER_OVER;
			AllocSvrConnect::getInstance()->userUpdateStatus(player, STATUS_PLAYER_OVER);
			player->startnum++;
			//结束的时候把此用户最终手牌类型算出来
			if(player->m_bHasCard)
			{
				//只有真的是走到第五轮的时候才会计算此用户的最终牌型
				if(table->m_bCurrRound != 5)
					player->m_nEndCardType = -1;
			}
		}
	}
	table->m_nStatus = STATUS_TABLE_OVER;
	table->setEndTime(time(NULL));

	AllocSvrConnect::getInstance()->updateTableStatus(table);
	table->calcCoin();
	IProcess::GameOver(table);
	table->reSetInfo();
	return 0;
}



