#include "PlayerTimer.h"
#include "BaseProcess.h"
#include "Player.h"
#include "Logger.h"
#include "Configure.h"

#include "RobotDefine.h"
#include "Protocol.h"
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
	stopLeaveTimer();
	stopHeartTimer();
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

void PlayerTimer::startHeartTimer(int uid,int timeout)
{
	m_HeartTimer.SetTimeEventObj(this, HEART_TIMER, uid);
	m_HeartTimer.StartTimer(timeout);
}

void PlayerTimer::stopHeartTimer()
{
	m_HeartTimer.StopTimer();
}

void PlayerTimer::startGameStartTimer(int uid,int timeout)
{
	m_BetTimer.SetTimeEventObj(this, START_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopGameStartTimer()
{
	m_BetTimer.StopTimer();
}

void PlayerTimer::startBetCoinTimer(int uid,int timeout)
{
	m_BetTimer.SetTimeEventObj(this, BET_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopBetCoinTimer()
{
	m_BetTimer.StopTimer();
}

void PlayerTimer::startCheckTimer(int uid,int timeout)
{
	m_CheckTimer.SetTimeEventObj(this, CHECK_TIMER, uid);
	m_CheckTimer.StartTimer(timeout);
}

void PlayerTimer::stopCheckTimer()
{
	m_CheckTimer.StopTimer();
}

int PlayerTimer::ProcessOnTimerOut(int Timerid, int uid)
{
	switch (Timerid)
	{
	case LEAVE_TIMER:
		return LeaveTimerTimeOut();
	case HEART_TIMER:
		return HeatTimeOut(uid);
	case START_TIMER:
		return StartGameTimeOut(uid);
	case BET_TIMER:
		return BetTimeOut(uid);
	case CHECK_TIMER:
		return CheckTimeOut(uid);
	default:
		return 0;
	}
	return 0;
}

int PlayerTimer::LeaveTimerTimeOut()
{
	this->stopLeaveTimer();
	player->logout();
	return 0;
}


int PlayerTimer::StartGameTimeOut(int uid)
{
	this->stopGameStartTimer();
	if(player->id != uid)
	{
		LOGGER(E_LOG_WARNING) << "StartGameTimeOut player uid=" << player->id << " is not this uid=" << uid;
		return -1;
	}
	player->gamestart();
	return 0;
}

int PlayerTimer::HeatTimeOut(int uid)
{
	this->stopHeartTimer();
	if(player->id != uid)
	{
		LOGGER(E_LOG_WARNING) << "HeatTimeOut player uid=" << player->id << " is not this uid=" << uid;
		return -1;
	}
	player->heartbeat();
	this->startHeartTimer(uid, 30);
	return 0;
}

int PlayerTimer::CheckTimeOut(int uid)
{
	this->stopCheckTimer();
	if(player->id != uid)
	{
		LOGGER(E_LOG_WARNING) << "CheckTimeOut player uid=" << player->id << " is not this uid=" << uid;
		return -1;
	}
	if(player->isCanGameOver())
		return 0;
	int randnum = rand()%10;

	if(player->hasSomeCheck() && randnum < 8)
		return player->check();
	if(player->robotKonwIsLargest() && randnum < 3)
		return player->check();
	if(!player->robotKonwIsLargest() && randnum < 6)
		return player->check();
	return 0;
}


int PlayerTimer::BetTimeOut(int uid)
{
	this->stopBetCoinTimer();
	if(player->id != uid)
	{
		LOGGER(E_LOG_WARNING) << "BetTimeOut player uid=" << player->id << " is not this uid=" << uid;
		return -1;
	}

	if (player->hasSomeCheck())
	{
		player->startCheckTimer(uid, 2+rand()%4);
	}

	/*int switchknowcoin = RobotRedis::getInstance().getRobotWinCount(player->clevel);
	bool bknow = false;
	switch ( player->clevel)
	{
	case E_PRIMARY_LEVEL:		
		if (switchknowcoin < Configure::getInstance().swtichWin1)
			bknow = true; 
		break;

	case E_MIDDLE_LEVEL:
		if (switchknowcoin < Configure::getInstance().swtichWin2)
			bknow = true;
		break;
		
	case E_ADVANCED_LEVEL:
		if (switchknowcoin < Configure::getInstance().swtichWin3)
			bknow = true;
		break;
			
	case E_MASTER_LEVEL:
		if (switchknowcoin < Configure::getInstance().swtichWin3)
			bknow = true;
		break;
	
	default:
		LOGGER(E_LOG_WARNING) << "robot unknown level=" << player->clevel;
		bknow = true;
		break;
	}*/

	/*if (bknow)
		return KnowProcess();
	else*/
		return UnKnowProcess();
	/*return 0;*/
}

int PlayerTimer::KnowProcess()
{
	if (player->robotKonwIsLargest())
	{
		player->setRaseCoin();

		int randnum = rand()%10;
		if(player->hascard == 1)
		{
			if(player->rasecoin > 0)
			{
				bool brase = false;
				if(player->CardType > 1  && player->CardType != 7 && randnum < 3)
					brase = true;

				if(brase)
				{
					if(player->optype & OP_RASE)
						return player->rase();
				}
			}
		}

		if(player->hascard == 2)
		{
			bool brase = false;
			if(player->CardType > 1 && player->CardType != 7 && randnum < 8)
				brase = true;
			if(brase)
			{
				if(player->rasecoin > 0)
				{
					if(player->optype & OP_RASE)
						return player->rase();
				}
			}
		}

		short tempround = 1 + rand()%3;
		if (player->currRound >= tempround)
		{
			if (player->currmax == player->rasecoinarray[0])
			{
				if (randnum < 6)
				{
					player->rasecoin = player->rasecoinarray[rand()%2 + 1];
					if(player->optype & OP_RASE)
						return player->rase();
				}
			}
		}


		if(player->CardType > 2 && player->CardType != 7)
		{
			if(player->hascard == 2 && randnum < 8)
			{
				if(player->optype & OP_ALLIN)
					return player->allin();
			}

			if(player->hascard == 1 && randnum < 3)
			{
				if(player->optype & OP_ALLIN)
					return player->allin();
			}
		}
		
		if(player->hascard == 2)
		{
			if(player->CardType == 1)
			{
				short round = 3 + rand()%6;
				if(player->currRound > round)
				{
					if(player->m_GameLogic.GetCardLogicValue(player->maxValue) < 0x0D)
					{
						if(player->optype & OP_THROW)
							return player->throwcard();
					}
					if(player->optype & OP_COMPARE)
						return player->comparecard();
				}
			}
			else if (player->CardType != 7)
			{
				short round = 9 + rand()%10;
				if(player->currRound > round)
					if(player->optype & OP_COMPARE)
						return player->comparecard();
			}
		}
		else
		{
			short round = 3 + rand()%6;
			if(player->currRound > round)
			{
				player->startBetCoinTimer(player->id, 2);
				return player->check();
			}
		}

		if(player->optype & OP_CALL)
			return player->call();
		if(player->optype & OP_ALLIN)
			return player->allin();
		if(player->optype & OP_THROW)
			return player->throwcard();
	}
	else
	{
		short randnum = rand()%10;
		if (player->CardType == 1)
		{
			if(player->isRaseMax())
			{
				if(player->hascard == 2 && randnum < 8)
					if(player->optype & OP_THROW)
						return player->throwcard();
			}
		}

		if(player->clevel == 1)
		{
			short tempround = 1 + rand()%3;
			if(player->currRound >= tempround)
			{
				if (player->currmax == player->rasecoinarray[0])
				{
					if(randnum < 4)
					{
						player->rasecoin = player->rasecoinarray[rand()%2 + 1];
						if(player->optype & OP_RASE)
							return player->rase();
					}
				}
			}
		}

		int round = 1 + rand()%5;
		if(player->clevel == 2)
			round = 1;
		if(player->currRound <= round)
		{
			if(player->optype & OP_CALL)
				return player->call();
		}

		if (player->hascard == 1)
		{
			player->startBetCoinTimer(player->id, 2);
			return player->check();
		}

		if (player->CardType > 1 && player->CardType != 7)
		{
			round = 3 + rand()%10;
			if(player->currRound < round)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
		}

		if (player->currRound > round && randnum < 4)
		{
			if(player->CardType == 1)
			{
				if(player->m_GameLogic.GetCardLogicValue(player->maxValue) < 0x0D)
				{
					if(player->optype & OP_THROW)
						return player->throwcard();
				}
			}
			if(player->optype & OP_COMPARE)
				return player->comparecard();
		}

		if(player->CardType > 1 && player->CardType != 7)
		{
			if(player->optype & OP_COMPARE)
				return player->comparecard();
		}

		if(player->optype & OP_THROW)
			return player->throwcard();
	}
	return 0;
}

int PlayerTimer::UnKnowProcess()
{
	int64_t tempbetcoin = player->sumbetcoin - player->ante;
	short wTemp;
	short round = 4 + rand()%6;
	if (tempbetcoin <= 0)	//其他闲家首次操作	//加注//跟注//看牌
	{
		//是否看牌 80%看牌
		if (player->hascard == 1)
		{
			if (player->currRound >= LIMIT_ROBOT_CHECK_ROUND)
			{
				wTemp=rand()%10;
				if(wTemp<8)
				{
					player->startBetCoinTimer(player->id, 2);
					return player->check();
				} 
			}
			else
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
		}
		else	//单牌随机放弃 20%放弃
		{
			if(player->CardType==1)
			{
				wTemp=rand()%10;
				if(wTemp<3)
				{
					return player->throwcard();
				}
			}
		}

		if(player->hascard == 2)
		{
			wTemp=rand()%10;
			if(player->CardType==1 && wTemp<5)
			{
				if(player->m_GameLogic.GetCardLogicValue(player->maxValue) < 0x0D)
				{
					if(player->optype & OP_THROW)
						return player->throwcard();
				}
				if(player->optype & OP_COMPARE)
					return player->comparecard();
			}
		}

		player->setRaseCoin();
		wTemp=rand()%10;
		if(player->rasecoin > 0 && wTemp < 5)
		{
			if(player->optype & OP_RASE)
				return player->rase();
		}

		if(player->optype & OP_CALL)
			return player->call();

		if(player->optype & OP_THROW)
			return player->throwcard();
	}
	else	//普通操作
	{
		if(player->hascard == 1)	//暗牌	//加注//跟注//比牌
		{
			//暗牌 10%看牌
			wTemp=rand()%10;
			if(player->currRound >= LIMIT_ROBOT_CHECK_ROUND)
			{
				if(wTemp<1)
				{
					player->startBetCoinTimer(player->id, 2);
					return player->check();
				}
			}
			else
			{
				if(player->optype & OP_CALL)
					return player->call();
			}

			//0加注//1跟注 //暗牌下注小于[封顶数目*2]加注 大于比牌
			if(tempbetcoin< player->rasecoinarray[3]*2 && (4*player->countnum*player->rasecoinarray[3]+player->currmax+tempbetcoin)<player->money)	
			{
				player->setRaseCoin();

				if(player->rasecoin > 0 && wTemp < 5)
				{
					if(player->optype & OP_RASE)
						return player->rase();
				}

				if(player->optype & OP_CALL)
					return player->call();

				if(player->optype & OP_THROW)
					return player->throwcard();
			}
			else	//比牌
			{
				if(player->optype & OP_COMPARE)
					return player->comparecard();
				if(player->optype & OP_THROW)
					return player->throwcard();
			}
		}
		else	//明牌	//加注//跟注//比赛//放弃
		{
			if(player->CardType==1 && tempbetcoin<=player->rasecoinarray[3]*5)	//单牌并且下注少于[封顶数目*5]随机放弃 20%放弃
			{
				wTemp=rand()%10;
				if(wTemp<1)
				{
					if(player->optype & OP_THROW)
						return player->throwcard();
				}
			}


			//加注//跟注 //明牌下注小于[封顶数目*牌型]加注 大于比牌
			if((player->CardType<4 && tempbetcoin<(1 * 2 * player->rasecoinarray[3] * player->CardType) || 
				player->CardType==4 && tempbetcoin<(3 * 2 * player->rasecoinarray[3] * player->CardType) || 
				player->CardType>4 && tempbetcoin<(5 * 2 * player->rasecoinarray[3] * player->CardType)) && 
				(4*player->countnum*player->rasecoinarray[3]+player->currmax+tempbetcoin)<player->money)
			{
				
				player->setRaseCoin();
				if(player->rasecoin > 0)
				{
					if(player->optype & OP_RASE)
						return player->rase();
				}

				if(player->optype & OP_ALLIN)
					return player->allin();
				if(player->optype & OP_CALL)
					return player->call();

				if(player->optype & OP_THROW)
					return player->throwcard();
			}
			else//比牌
			{
				if(player->CardType==1)
				{
					if(player->m_GameLogic.GetCardLogicValue(player->maxValue) < 0x0D)
					{
						if(player->optype & OP_THROW)
							return player->throwcard();
					}
				}

				if(player->currRound < round)
				{
					if(player->CardType > 1 && player->CardType != 7 && wTemp < 50)
					{
						if(player->optype & OP_CALL)
							return player->call();
					}
				}

				if(player->optype & OP_COMPARE)
					return player->comparecard();
				if(player->optype & OP_THROW)
					return player->throwcard();
			}
			if(player->optype & OP_THROW)
				return player->throwcard();
		}
	}
	return 0;
}
