#include "PlayerTimer.h"
#include "IProcess.h"
#include "Player.h"
#include "Logger.h"
//====================CTimer_Handler==================================//

int CTimer_Handler::ProcessOnTimerOut()
{
	if(handler)
		return handler->ProcessOnTimerOut(this->timeid, this->uid);
	else
		return 0;

}

void CTimer_Handler::SetTimeEventObj(PlayerTimer * obj, int timeid, int uid)
{
	this->handler = obj;
	this->timeid = timeid;
	this->uid = uid;
}


//==========================PlayerTimer==================================//

void PlayerTimer::init(Player* player)
{
	this->player = player;
}


void PlayerTimer::stopAllTimer()
{
	stopBetCoinTimer();
	stopLeaveTimer();
	stopActiveLeaveTimer();
	stopHeartTimer();
}

void PlayerTimer::startBetCoinTimer(int uid,int timeout)
{
	m_BetTimer.SetTimeEventObj(this, BET_COIN_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopBetCoinTimer()
{
	m_BetTimer.StopTimer();
}

void PlayerTimer::startLeaveTimer(int timeout)
{
	m_LeaveTimer.SetTimeEventObj(this, LEAVE_TIMER);
	m_LeaveTimer.StartTimer(timeout);
}

void PlayerTimer::stopLeaveTimer()
{
	m_LeaveTimer.StopTimer();
}

void PlayerTimer::startActiveLeaveTimer(int timeout)
{
	m_ActiveLeaveTimer.SetTimeEventObj(this, ACTIVE_LEAVE_TIMER);
	m_ActiveLeaveTimer.StartTimer(timeout);
}

void PlayerTimer::stopActiveLeaveTimer()
{
	m_ActiveLeaveTimer.StopTimer();
}

void PlayerTimer::startHeartTimer(int uid,int timeout)
{
	m_HeartTimer.SetTimeEventObj(this, HEART_TIMER, uid);
	m_HeartTimer.StartTimer(timeout);
}

void PlayerTimer::stopHeartTimer()
{
	m_HeartTimer.StopTimer();
}

int PlayerTimer::ProcessOnTimerOut(int Timerid, int uid)
{
	switch (Timerid)
	{
	case BET_COIN_TIMER:
		return BetTimerTimeOut(uid);
	case LEAVE_TIMER:
		return LeaveTimerTimeOut();
	case ACTIVE_LEAVE_TIMER:
		return ActiveLeaveTimeOut();
	case HEART_TIMER:
		return HeatTimeOut(uid);
	default:
		return 0;
	}
	return 0;
}

int PlayerTimer::knowProcess(int uid)
{
	if(player->robotKonwIsLargest())
	{
		int num = rand()%100;
		if(num < 60)
		{
			if(player->optype & OP_ALLIN)
			{
				return player->allin();
			}
		}

		if(player->optype & OP_RASE)
		{
			int willrase = rand()%1000;
			player->setKnowRaseCoin();

			if(player->betCoinList[player->currRound] != 0)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}

			if(player->currRound < 3)
			{
				return player->rase();
			}

			if (willrase < 800)
			{
				return player->rase();
			}
		}
		if(player->optype & OP_ALLIN)
		{
			int willallin = rand()%1000;
			if(willallin < 950)
			{
				if(player->currcardvalue > 0)
					return player->allin();
			}
		}
		if(player->optype & OP_CALL)
		{
				return player->call();
		}
		if(player->optype & OP_CHECK)
			return player->check();
	}

	else
	{
		
		if(player->currRound >= 4)
		{
			if(player->robotIsLargest())
			{
				if(player->optype & OP_CHECK)
					return player->check();
			}
			else
			{

				if(player->optype & OP_THROW)
					return player->throwcard();
			}
		}

		if(player->currRound <= 2)
			return notKonwProcess(uid);

		if(player->clevel <= 2)
		{
			if(player->diffcoin <= player->ante && player->betCoinList[player->currRound] == 0)
			{
				int willcall = rand()%100;
				if(willcall < 80)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
			}
		}

		if(player->optype & OP_CHECK)
			return player->check();

		if(player->optype & OP_CALL)
		{
			if(player->currRound < 2)
			{
				if(player->m_GameLogic.GetCardValoe(player->card_array[0]) < 10 || 
					player->m_GameLogic.GetCardValoe(player->card_array[1]) < 10)
					return player->throwcard();
				return player->call();
			}
		}
		if(player->optype & OP_THROW)
			return player->throwcard();
	}

	if(player->optype & OP_CHECK)
		return player->check();
	if(player->optype & OP_THROW)
		return player->throwcard();
	return 0;
}

int PlayerTimer::BetTimerTimeOut(int uid)
{
	this->stopBetCoinTimer();
	if(player->id != uid)
	{
		LOGGER(E_LOG_WARNING)<< "player uid["<<player->id<<"] is not this uid["<<uid<<"]";
		//_LOG_ERROR_("player uid[%d] is not this uid[%d]\n", player->id, uid);
		return -1;
	}
	if(player->isKnow)
		return knowProcess(uid);
	return notKonwProcess(uid);
	
	return 0;
}

int PlayerTimer::notKonwProcess(int uid)
{
	//机器人在当前是最大的牌，包括机器人自己的底牌
	if(player->robotIsLargest())
	{
		//_LOG_DEBUG_("larger++++++tid[%d]++++++++player->currcardvalue:%d++++++++++\n",player->id,player->currcardvalue);
		if(player->optype & OP_RASE)
		{
			int willrase = rand()%1000;
				player->setRaseCoin();

			if(player->betCoinList[player->currRound] != 0)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}

			if(player->currRound < 3)
			{
				return player->rase();
			}
		
			if (willrase < 800)
			{
				return player->rase();
			}

			if(player->currcardvalue > 0)
				return player->rase();
			
		}
		if(player->optype & OP_ALLIN)
		{
			int willallin = rand()%1000;
			if(willallin < 950)
			{
				if(player->currcardvalue > 0)
					return player->allin();
			}
		}
		if(player->optype & OP_CALL)
		{
			if(player->currRound < 3)
			{
				if(player->diffcoin < player->ante * 10)
					return player->call();
			}
			if(player->currcardvalue > 0)
				return player->call();

		}
		if(player->optype & OP_CHECK)
			return player->check();
	}
	else
	{
		//_LOG_DEBUG_("small++++++tid[%d]++++++++player->currcardvalue:%d++++++++++\n",player->id,player->currcardvalue);
		/*if(player->currRound ==5)
		{
			if(player->optype & OP_THROW)
				return player->throwcard();
		}*/
		if(player->optype & OP_CHECK)
			return player->check();
		if(player->optype & OP_CALL)
		{
			int willcall = rand()%1000;
			if(player->currRound == 3)
			{
				if(willcall < 100 && player->currcardvalue > 0)
					return player->call();
			}
			if(player->currRound < 2)
			{
				if(player->currcardvalue > 0)
					return player->call();
				
				if (willcall < 900)
				{
					if(player->clevel > 2)
					{
						if(player->m_GameLogic.GetCardValoe(player->card_array[0]) < 10 || 
							player->m_GameLogic.GetCardValoe(player->card_array[1]) < 10)
							return player->throwcard();
					}
					return player->call();
				}
			}
		}
		if(player->optype & OP_THROW)
			return player->throwcard();
	}

	if(player->optype & OP_CHECK)
		return player->check();
	if(player->optype & OP_THROW)
		return player->throwcard();
	return 0;
}

int PlayerTimer::LeaveTimerTimeOut()
{
	this->stopLeaveTimer();
	//_LOG_DEBUG_("player status[%d]\n", player->status);
	///if(player->status == STATUS_TABLE_OVER)
	//{
	//	return 0;
	//}
	player->logout();
	return 0;
}

int PlayerTimer::ActiveLeaveTimeOut()
{
	this->stopActiveLeaveTimer();
	//_LOG_DEBUG_("ActiveLeaveTimeOut player status[%d]\n", player->status);
	if(player->status != STATUS_PLAYER_ACTIVE)
	{
		if(player->status == STATUS_TABLE_OVER)
		{
			if(rand()%10 < 2)
			{
				player->logout();
				return 0;
			}
		}
		else
		{
			if(rand()%10 < 3)
			{
				player->logout();
				return 0;
			}
		}
		this->startActiveLeaveTimer(5+rand()%4);
	}
	return 0;
}

int PlayerTimer::HeatTimeOut(int uid)
{
	this->stopHeartTimer();
	if(player->id != uid)
	{
		LOGGER(E_LOG_WARNING)<< "HeatTimeOut player uid["<<player->id<<"] is not this uid["<<uid<<"]";
	//	_LOG_ERROR_("HeatTimeOut player uid[%d] is not this uid[%d]\n", player->id, uid);
		return -1;
	}
	player->heartbeat();
	this->startHeartTimer(uid, 30);
	return 0;
}
