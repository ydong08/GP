#include "TableTimer.h"
#include "IProcess.h"
#include "Table.h"
#include "Logger.h"
#include "Configure.h"
#include "HallManager.h"
#include "AllocSvrConnect.h"
#include "CoinConf.h"
#include "GameUtil.h"
#include "ProtocolServerId.h"
#include "GameApp.h"
#include "Util.h"

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
	m_BetTimer.StopTimer();
}

void TableTimer::startIdleTimer(int timeout)
{
	m_BetTimer.SetTimeEventObj(this, IDLE_TIMER);
	m_BetTimer.StartTimer(timeout);
}

void TableTimer::stopIdleTimer()
{
	m_BetTimer.StopTimer();
}

void TableTimer::startBetTimer(int timeout)
{
	m_BetTimer.SetTimeEventObj(this, BET_TIMER);
	m_BetTimer.StartTimer(timeout);
}

void TableTimer::stopBetTimer()
{
	m_BetTimer.StopTimer();
}

void TableTimer::startOpenTimer(int timeout)
{
	m_BetTimer.SetTimeEventObj(this, OPEN_TIMER);
	m_BetTimer.StartTimer(timeout);
}

void TableTimer::stopOpenTimer()
{
	m_BetTimer.StopTimer();
}

void TableTimer::startSettlementTimer(int timeout)
{
	m_TableNormalTimer.SetTimeEventObj(this, TABLE_CONFIRM_TIMER);
	m_TableNormalTimer.StartTimer(timeout);
}

void TableTimer::stopSettlementTimer()
{
	m_TableNormalTimer.StopTimer();
}

int TableTimer::SettlementTimeOut()
{
	this->stopSettlementTimer();
	table->GameStart();	// 要先改状态，不然前一个状态会下发两次
	IProcess::sendTableStatus(table, Configure::getInstance()->ready_time);
	return 0;
}

int TableTimer::ProcessOnTimerOut(int Timerid, int uid)
{
	switch (Timerid)
	{
	case IDLE_TIMER:
		return IdleTimeOut();
	case BET_TIMER:
		return BetTimeOut();
	case OPEN_TIMER:
		return OpenTimeOut();
	case TABLE_CONFIRM_TIMER:
		return SettlementTimeOut();
	default:
		return 0;
	}
	return 0;
}

int TableTimer::IdleTimeOut()
{
	//_LOG_INFO_("IdleTimeOut\n");
	startBetTimer(Configure::getInstance()->bettime);
	table->m_nStatus = STATUS_TABLE_BET;

	IProcess::ResetAllPlayerState(table, STATUS_PLAYER_ACTIVE);

    table->setGameID(NULL);
	table->setStatusTime(time(NULL));
	IProcess::sendTableStatus(table, Configure::getInstance()->bettime);
	//int interval = Util::rand_range(100, 300);
	//startSendTimer(interval);

	return 0;
}

int TableTimer::BetTimeOut()
{
	//_LOG_INFO_("BetTimeOut\n");
	startOpenTimer(Configure::getInstance()->opentime);
	table->m_nStatus = STATUS_TABLE_OPEN;
	table->setStatusTime(time(NULL));
	IProcess::sendTableStatus(table, Configure::getInstance()->opentime);
	table->setEndTime(time(NULL));
	if(table->m_bHasBet)
	{
        table->m_bHasBet = false;

        for (int idx = 1; idx < AREA_MAX; ++idx)
        {
            CoinConf::getInstance()->setPlayerBet(idx, table->m_lRealBetArray[idx]);
        }

		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			Player* player = table->player_array[i];
            if (player == NULL || player->source == 30 || player->m_lBetArray[0] <= 0)
            {
                continue;
            }

            for (int idx = 1; idx < AREA_MAX; ++idx)
            {
                int64_t betcoin_of_this_type = player->m_lBetArray[idx];
                if (betcoin_of_this_type <= 0)
                {
                    continue;
                }

                CoinConf::getInstance()->setPlayerUidBet(idx, player->id, betcoin_of_this_type);
            }
		}
	}
	table->GameOver();
	return 0;
}

int TableTimer::OpenTimeOut()
{
	startSettlementTimer(Configure::getInstance()->ready_time);

	short pid = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = table->player_array[i];
		if(player)
		{
			int64_t winmoney = player->m_lWinScore /*- player->m_lBetArray[0]*/ + player->m_lLostScore;
			std::string strTrumpt;
			pid = player->pid;
			if (GameUtil::getDisplayMessage(strTrumpt, GAME_ID, Configure::getInstance()->m_nLevel, player->name, winmoney, Configure::getInstance()->wincoin1,
				Configure::getInstance()->wincoin2, Configure::getInstance()->wincoin3, Configure::getInstance()->wincoin4))
			{
				AllocSvrConnect::getInstance()->trumptToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), player->pid);
			}
		}
	}

	if (table->isNotFull())
	{
		std::string	strTrumpt;
		if (GameUtil::getInvitePlayerTrumpt(strTrumpt, GAME_ID, Configure::getInstance()->idletime))
			AllocSvrConnect::getInstance()->trumptToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), pid);
	}

	table->m_nStatus = STATUS_TABLE_SETTLEMENT;
	IProcess::sendTableStatus(table, Configure::getInstance()->ready_time);
	IProcess::NotifyGameResult(table);

	table->setStatusTime(time(NULL));
	table->setStartTime(time(NULL));
	table->reset();
	table->reloadCfg();
	
	CoinConf::getInstance()->setPlayerBet(1,table->m_lRealBetArray[1]);
	CoinConf::getInstance()->setPlayerBet(2,table->m_lRealBetArray[2]);
	CoinConf::getInstance()->setPlayerBet(3,table->m_lRealBetArray[3]);
	CoinConf::getInstance()->clearPlayerUidBet();

	return 0;
}