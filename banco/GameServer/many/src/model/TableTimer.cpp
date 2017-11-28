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

void TableTimer::startSendTimer(int timeout)
{
	m_SendTimer.SetTimeEventObj(this, SEND_BET_TIMER);
	m_SendTimer.StartMilliTimer(timeout);
}

void TableTimer::stopSendTimer()
{
	m_SendTimer.StopTimer();
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
	case SEND_BET_TIMER:
		return SendBetInfoTimeOut();
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

	time_t t;       
    time(&t);               
	char time_str[32]={0};
    struct tm* tp= localtime(&t);
    strftime(time_str,32,"%Y%m%d%H%M%S",tp);
    char gameId[64] ={0};
	Player* banker = NULL;
	if(table->bankersid >=0)
		banker = table->player_array[table->bankersid];
    sprintf(gameId, "%s|%d|%02d", time_str, banker ? banker->id: 0, table->bankernum + 1);
    table->setGameID(gameId);
	table->setStatusTime(time(NULL));
	IProcess::sendTableStatus(table, Configure::getInstance()->bettime);
	int interval = Util::rand_range(100, 300);
	startSendTimer(interval);

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
		CoinConf::getInstance()->setPlayerBet(1,table->m_lRealBetArray[1]);
		CoinConf::getInstance()->setPlayerBet(2,table->m_lRealBetArray[2]);
		CoinConf::getInstance()->setPlayerBet(3,table->m_lRealBetArray[3]);
		table->m_bHasBet = false;
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			Player* player = table->player_array[i];
			if(player && player->source != 30 && player->m_lBetArray[0] > 0)
			{
				if(player->m_lBetArray[1] > 0)
					CoinConf::getInstance()->setPlayerUidBet(1, player->id, player->m_lBetArray[1]);
				if(player->m_lBetArray[2] > 0)
					CoinConf::getInstance()->setPlayerUidBet(2, player->id, player->m_lBetArray[2]);
				if(player->m_lBetArray[3] > 0)
					CoinConf::getInstance()->setPlayerUidBet(3, player->id, player->m_lBetArray[3]);
			}
		}
	}
	table->GameOver();
	stopSendTimer();
	return 0;
}

int TableTimer::OpenTimeOut()
{
	//_LOG_INFO_("OpenTimeOut\n");
	startIdleTimer(Configure::getInstance()->idletime);
	short sendnum = 0;
	short pid = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->m_nCountPlayer == sendnum)
			break;
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
			sendnum++;
		}
	}

	{
		std::string	strTrumpt;
		if (GameUtil::getInvitePlayerTrumpt(strTrumpt, GAME_ID, Configure::getInstance()->idletime))
			AllocSvrConnect::getInstance()->trumptToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), pid);
	}

	table->m_nStatus = STATUS_TABLE_IDLE;
	table->setStatusTime(time(NULL));
	table->setStartTime(time(NULL));
	table->reset();
	table->reloadCfg();
	IProcess::sendTableStatus(table, Configure::getInstance()->idletime);
	
	CoinConf::getInstance()->setPlayerBet(1,table->m_lRealBetArray[1]);
	CoinConf::getInstance()->setPlayerBet(2,table->m_lRealBetArray[2]);
	CoinConf::getInstance()->setPlayerBet(3,table->m_lRealBetArray[3]);
	CoinConf::getInstance()->clearPlayerUidBet();

	return 0;
}

int TableTimer::SendBetInfoTimeOut()
{
	//_LOG_INFO_("SendBetInfoTimeOut\n");
	stopSendTimer();
	int interval = Util::rand_range(100, 300);
	startSendTimer(interval);
	int sendnum = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
		Player* p = table->player_array[i];
		if(table->betVector.size() < 1)
			return 0;
		if(p)
		{
			OutputPacket response;
			response.Begin(GMSERVER_MSG_NOTBETINFO, p->id);
			response.WriteShort(0);
			response.WriteString("ok");
			response.WriteInt(p->id);
			response.WriteShort(p->m_nStatus);
			response.WriteInt(table->id);
			response.WriteShort(table->m_nStatus);
			for(int j = 1; j < BETNUM; ++j)
				response.WriteInt64(table->m_lTabBetArray[j]);
			vector<BetInfo>::iterator it;
			int size = 0;
			it = table->betVector.begin();
			while(it != table->betVector.end())
			{
				if((*it).nUid != p->id)
					size++;
				it++;
			}

			if(size > Configure::getInstance()->switchsendsize)
			{
				response.WriteShort(Configure::getInstance()->switchsendsize);
				it = table->betVector.begin();
				int index = 0;
				while(it != table->betVector.end())
				{
					if(index == Configure::getInstance()->switchsendsize)
						break;
					BetInfo other = *it;
					if(other.nUid != p->id)
					{
						index++;
						response.WriteInt(other.nUid);
						response.WriteShort(other.nSid);
						response.WriteByte(other.bType);
						response.WriteInt(other.nBetCoin);
					}
					it++;
				}
			}
			else
			{
				response.WriteShort(size);
				it = table->betVector.begin();
				while(it != table->betVector.end())
				{
					BetInfo other = *it;
					if(other.nUid != p->id)
					{
						response.WriteInt(other.nUid);
						response.WriteShort(other.nSid);
						response.WriteByte(other.bType);
						response.WriteInt(other.nBetCoin);
					}
					it++;
				}
			}
            for(int k = 1; k < BETNUM; ++k)
            {
                response.WriteInt64(table->betAreaLimit[k]);
            }
			response.End();
			//_LOG_DEBUG_("size:%d\n", size);
			if(size > 0)
			{
                //ULOGGER(E_LOG_INFO, p->id) << "send bet info now!count player = " << table->m_nCountPlayer;
				if(HallManager::getInstance()->sendToHall(p->m_nHallid, &response, false) < 0)
					_LOG_ERROR_("[SendBetInfoTimeOut] Send To Uid[%d] Error!\n", p->id);
			}

			table->SendSeatInfo(p);
			sendnum++;
		}
	}
	table->betVector.clear();
	if(table->m_bHasBet)
	{
        for (int j = 1; j < BETNUM; j++)
        {
            CoinConf::getInstance()->setPlayerBet(j,table->m_lRealBetArray[j]);
        }
		table->m_bHasBet = false;
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			Player* p = table->player_array[i];
			if(p && p->source != E_MSG_SOURCE_ROBOT && p->m_lBetArray[0] > 0)
			{
                for (int j = 1; j < BETNUM; j++)
                {
                    if (p->m_lBetArray[j] > 0)
                    {
                        CoinConf::getInstance()->setPlayerUidBet(j, p->id, p->m_lBetArray[j]);
                    }
                }
			}
		}
	}
	return 0;
}
