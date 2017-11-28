#include "PlayerTimer.h"
#include "IProcess.h"
#include "Player.h"
#include "Logger.h"
#include "RobotRedis.h"
#include "Configure.h"
#include "AIDo.h"
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

void PlayerTimer::startStartGameTimer(int timeout)
{
	m_StartTimer.SetTimeEventObj(this, START_TIMER);
	m_StartTimer.StartTimer(timeout);
}

void PlayerTimer::stopStartGameTimer()
{
	m_StartTimer.StopTimer();
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
	case START_TIMER:
		return StartTimeOut();
	default:
		return 0;
	}
	return 0;
}


int PlayerTimer::BetTimerTimeOut(int uid)
{
	this->stopBetCoinTimer();
	if(player->id != uid)
	{
		LOGGER(E_LOG_DEBUG)<< "player uid["<<player->id<<"] is not this uid["<<uid<<"]";
		//_LOG_ERROR_("player uid[%d] is not this uid[%d]\n", player->id, uid);
		return -1;
	}

	short level = player->clevel;
	short loadtype = 2;
	if(level <= LEVEL1)
		loadtype = 1;
	else if (level > LEVEL1 && level <= LEVEL2)
		loadtype = 2;
	else if (level > LEVEL2 && level <= LEVEL3)
		loadtype = 3;
	else if (level >= LEVEL6)
		loadtype = 6;

	int switchknowcoin = 0;
	switchmoney = RobotRedis::getInstance()->getRobotWinCount(loadtype);
	if ( loadtype == 1 )
	{
		switchmoneyup = Configure::instance()->swtichWin1;
		switchknowcoin = Configure::instance()->swtichOnKnow1;
	}
	else if (loadtype == 2 )
	{
		switchmoneyup = Configure::instance()->swtichWin2;
		switchknowcoin = Configure::instance()->swtichOnKnow2;
	}
	else if(loadtype == 6 )
	{
		switchmoneyup = Configure::instance()->swtichWin6;
		switchknowcoin = Configure::instance()->swtichOnKnow6;
	}

	printf("-----switchmoney:%d switchknowcoin:%d \n", switchmoney, switchknowcoin);
	if(switchmoney <= switchknowcoin)
	{
		if(switchmoney > switchmoneyup)
			return handleProcess(uid);
		else
			return knowProcess(uid);
	}

	AIDo* aiPlayer = AIDo::getInstance();
	BYTE mycard[2]={0,0};
	mycard[0] = player->card_array[0];
	mycard[1] = player->card_array[1];
	BYTE publicCard[5] = {0,0,0,0,0};
	for (int i = 0; i<5; i++)
	{
		if (player->CenterCard[i]!=0)
		{
			publicCard[i] = player->CenterCard[i];
		}
	}
	int action = aiPlayer->getAIDo(mycard, publicCard, player->PoolCoin, player->currMaxCoin - player->betCoinList[player->currRound],
		player->ante,player->getPlayNum());



	switch (action)
	{
		case AI_FOLD:
		{
			return player->throwcard();
			break;
		}
		case AI_CALL:
		{
			if(player->optype & OP_CALL)
				return player->call();
			
			if(player->optype & OP_CHECK)
				return player->check();

			if(player->optype & OP_ALLIN)
				return player->allin();
			break;

		}

		case AI_RAISE:
		{
			int64_t maxValue=player->limitcoin;
			int64_t callNeedMoney = player->currMaxCoin - player->betCoinList[player->currRound];
			int64_t miniValue =0;
			int64_t chipToRaiseMin = callNeedMoney*2;
			if (player->betCoinList[player->currRound]==player->ante*0.5)
			{
				if (callNeedMoney <= player->ante*0.5)
				{
					chipToRaiseMin = (int64_t)(player->ante*0.5+player->ante);
				}
				else
				{
					chipToRaiseMin = (int64_t)((callNeedMoney-player->ante*0.5)*2+player->ante*0.5);
				}
				
			}
			if (chipToRaiseMin==0)
			{
				int randindex = rand()%6;
				int64_t raisebackup=0;
				if (randindex>=0&&randindex<=2)
				{
					raisebackup = player->PoolCoin/3;
				}
				else if (randindex>=3&&randindex<=4)
				{
					raisebackup = player->PoolCoin/2;
				}
				else
				{
					raisebackup = player->PoolCoin*2/3;
				}
				int64_t num = raisebackup/player->ante;
				if (raisebackup%player->ante>0)
				{
					num++;
				}
				chipToRaiseMin = player->ante*num;

			}
			
			if (chipToRaiseMin>=player->limitcoin)
			{
				miniValue = player->limitcoin;
			}
			else
			{
				miniValue = chipToRaiseMin;
			}
			int64_t raiseChip = miniValue;
			int64_t gap = maxValue-miniValue;
			
			
			int64_t raiseGap = player->ante*2;
			int64_t raisebackup[5] = {0,0,0,0,0};
			for (int i = 0; i<5; i++)
			{
				int64_t raisenum =raiseGap*(i+1);
				if (raisenum>10000)
				{
					int left = raisenum%100;
					if (left>0)
					{
						raisenum = raisenum-left;
					}
				}
				if (raisenum<gap)
				{
					 raisebackup[i]=raisenum;
				}
				else
				{
					raisebackup[i] = gap;
				}
				
			}
			
			int randnum = rand()%100;
			if (randnum>70&&randnum<82)
			{
				raiseChip+=raisebackup[0];
			}
			else if (randnum>=82&&randnum<90)
			{
				raiseChip+=raisebackup[1];
			}
			else if (randnum>=90&&randnum<95)
			{
				raiseChip+=raisebackup[2];
			}
			else if (randnum>=95&&randnum<98)
			{
				raiseChip+=raisebackup[3];
			}
			else if (randnum>=98&&randnum<100)
			{
				raiseChip+=raisebackup[4];
			}
			player->rasecoin = raiseChip;
			return player->rase();
			break;
		}
			
		default:
			break;
	}

	/*if(player->isKnow)
	{
		_LOG_DEBUG_("---------knowProcess-------uid:%d isKnow:%s\n",player->id, player->isKnow ? "true" : "false");
		return knowProcess(uid);
	}
	else
	{
		_LOG_DEBUG_("---------notKonwProcess-------uid:%d isKnow:%s\n",player->id, player->isKnow ? "true" : "false");
		return notKonwProcess(uid);
	}*/
	
	return 0;
}

int PlayerTimer::knowProcess(int uid)
{
	if(player->robotKonwIsLargest())
	{
		//已经知道一定赢！！！！
		int num = rand()%100;
		short cardvalue1 = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
		short cardvalue2 = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
		short cardcolor1 = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
		short cardcolor2 = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
		BYTE bCardType = player->finalcardvalue;
		
		if(player->clevel != 8)
		{
			if(player->currRound == 1)
				return Round1Op();
		}

		if(player->currRound > 1)
		{
			bool isInHand = false;
			tagAnalyseResult AnalyseResult;
			player->m_GameLogic.AnalysebCardData(player->Knowcard_array,5,AnalyseResult);
			if( AnalyseResult.cbThreeCount > 0 && 
				(AnalyseResult.cbThreeLogicVolue[0] == cardvalue1 || AnalyseResult.cbThreeLogicVolue[0] == cardvalue2))
			{
				isInHand = true;
			}
			int randnum = rand()%100;
			//三条以上
			if( bCardType > CT_THREE_TIAO || ( bCardType == CT_THREE_TIAO && isInHand ) )
			{
				//梭哈
				if( bCardType > CT_TONG_HUA )
				{
					if( randnum < 20)
					{
						if( player->optype & OP_ALLIN )
							return player->allin();
					}
				}
				//加注
				if( player->optype & OP_RASE )
				{
					if( player->setRandRaseCoin(3, 6 ) == 0 )
					{
						if( player->optype & OP_ALLIN )
							return player->allin();
					}
					else
					{
						if(player->betCoinList[player->currRound] != 0)
						{
							if(player->optype & OP_CALL)
								return player->call();
						}
						return player->rase();
					}
				}
				//跟注
				if( player->optype & OP_CALL )
					return player->call();
			}
		}

		if( player->currMaxCoin == 0 && bCardType > CT_ONE_LONG )
		{
			int randnum = rand()%100;
			if( randnum < 60 )
			{
				if(player->optype & OP_RASE)
				{
					if( player->setRandRaseCoin( 1, 3 ) == 1 )
						return player->rase();
				}
			}
		}

		if( bCardType >= CT_THREE_TIAO )
		{
			if( player->currRound == 4 || num < 20 )
			{
				if(player->optype & OP_ALLIN)
					return player->allin();
			}
		}

		if( bCardType < CT_THREE_TIAO )
		{
			if((cardvalue1 >= 10 && cardvalue2 >= 10) ||
				(cardvalue1 > 5 && (cardvalue1 == cardvalue2)) || 
				((cardcolor1 == cardcolor2) && (cardvalue1 >= 13 || cardvalue2 >= 13)))
			{
				if( num < 40 )
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
			}
			else
			{
				if( (cardvalue1 > 10 && cardvalue2 > 10) || (cardcolor1 == cardcolor2 ) || cardvalue1 == 0x0E || cardvalue2 == 0x0E
				|| ((cardvalue1>10 || cardvalue2>10) && ((cardvalue1 - cardvalue2) == 1)))
				{
					if( player->optype & OP_CALL )
						return player->call();
				}

				if( num < 60 )
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
			}
		}

		if(player->optype & OP_RASE)
		{
			if((cardvalue1 >= 10 && cardvalue2 >= 10) ||
				(cardvalue1 > 5 && (cardvalue1 == cardvalue2)) || 
				((cardcolor1 == cardcolor2) && (cardvalue1 >= 13 || cardvalue2 >= 13)))
			{
				if( num < 30 )
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
			}

			int willrase = rand()%1000;
			if(player->currMaxCoin < player->limitcoin)
			{
				if( player->currRound < 3 )
				{
					if( willrase < 800 )
						player->setRandRaseCoin( 2, 5 );
					else
						player->setKnowRaseCoin();
				}
				else
				{
					if( willrase < 500 )
						player->setRandRaseCoin( 2, 8 );
					else
						player->setKnowRaseCoin();
				}
			}
			else if(player->optype & OP_ALLIN)
			{
				return player->allin();
			}

			if(player->betCoinList[player->currRound] != 0)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}

			if (willrase < 500)
			{
				return player->rase();
			}
		}
		if( (cardvalue1 > 10 && cardvalue2 > 10) || (cardcolor1 == cardcolor2 ) || cardvalue1 == 0x0E || cardvalue2 == 0x0E
				|| ((cardvalue1>10 || cardvalue2>10) && ((cardvalue1 - cardvalue2) == 1)))
		{
			if( player->currMaxCoin == 0)
			{
				int randnum = rand()%100;
				if( randnum < 60 )
				{
					if(player->optype & OP_RASE)
					{
						if( player->setRandRaseCoin( 1, 3 ) == 1 )
							return player->rase();
					}
				}
			}
			if(player->optype & OP_CALL)
			{
				return player->call();
			}
		}

		if( num < 50 )
		{
			if( player->currMaxCoin == 0)
			{
				int randnum = rand()%100;
				if( randnum < 60 )
				{
					if(player->optype & OP_RASE)
					{
						if( player->setRandRaseCoin( 1, 3 ) == 1 )
							return player->rase();
					}
				}
			}
			if(player->optype & OP_CALL)
				return player->call();
		}
		if(player->optype & OP_CHECK)
			return player->check();
		if(player->optype & OP_CALL)
			return player->call();
		if( player->optype & OP_ALLIN )
			return player->allin();
		if(player->optype & OP_THROW)
			return player->throwcard();
	}
	else
		return notWinProcess(uid);
	return 0;
}

int PlayerTimer::notWinProcess(int uid)
{
	//机器人一直在输，在无法赢时，只能看牌或弃牌
	if(player->optype & OP_CHECK)
		return player->check();

	if(player->optype & OP_THROW)
		return player->throwcard();

	return 0;
}

int PlayerTimer::handleProcess(int uid)
{
	if(player->currRound == 1)
		return Round1Op();
	if(player->currRound == 2)
		return Round2Op();
	if(player->currRound == 3)
		return Round3Op();
	if(player->currRound == 4)
		return Round4Op();
	return 0;
}

int PlayerTimer::Round1Op()
{
	short card1value = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short card2value = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	short cardcolor1 = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short cardcolor2 = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	if(card1value < card2value)
	{
		short temp = card1value;
		card1value = card2value;
		card2value = temp;
	}
	
	bool bLargest = player->robotKonwIsLargest();
	int randnum = rand()%100;

	printf("+++++++++++++++++++++=uid:%d=======card1:0x%02x card2:0x%02x\n", player->id, player->card_array[0], player->card_array[1]);

	if(card1value == card2value || (card1value == 0x0E && (card2value == 0x0D || card2value == 0x0C)))
	{
		if( randnum < 40 )
		{
			if( player->optype & OP_ALLIN )
				return player->allin();
		}
		else
		{
			if(player->betCoinList[player->currRound] != 0)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
			if((player->optype & OP_RASE))
			{
				if(player->setRandRaseCoin( 0, 8) > 0)
					return player->rase();
			}
		}
	}

	if( (card1value > 10 && card2value > 10) ||
		((card1value - card2value) == 1) || (cardcolor1 == cardcolor2 ) )
	{
		if(player->currMaxCoin == 0)
		{
			if(player->optype & OP_RASE)
			{
				if( player->setRandRaseCoin(0, 4) == 1 )
					return player->rase();
			}
		}
		else
		{
			if(player->currMaxCoin < player->ante*6)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
		}
		
		if(randnum < 30)
		{
			if(player->betCoinList[player->currRound] != 0)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
			if((player->optype & OP_RASE))
			{
				if(player->setRandRaseCoin( 1, 5) > 0)
					return player->rase();
			}
		}
	}

	if(switchmoney > switchmoneyup)
	{
		if(player->currMaxCoin <= player->ante*10)
		{
			if(player->optype & OP_CALL)
				return player->call();
		}
		else
		{
			if(bLargest)
			{
				if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
				{
					if(player->currMaxCoin > player->ante*15)
					{
						if(player->optype & OP_ALLIN)
							return player->allin();
					}
				}
				if(randnum < 80)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				int randallin = rand()%100;
				if(randallin < 10)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
			}
		}
		
		if(!bLargest)
		{
			int randrase = rand()%100;
			if(randrase < 50)
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				if((player->optype & OP_RASE))
				{
					if(player->setRandRaseCoin( 1, 5) > 0)
						return player->rase();
				}
			}
		}
	}
	else
	{
		if(bLargest)
		{
			if(player->betCoinList[player->currRound] != 0)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
			int randrase = rand()%100;
			if(randrase < 60)
			{
				if((player->optype & OP_RASE))
				{
					if(player->setRandRaseCoin( 0, 5) > 0)
						return player->rase();
				}
			}
			if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
			{
				if(player->currMaxCoin > player->ante*15)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
			}
			if(randnum < 80)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
			int randallin = rand()%100;
			if(randallin < 10)
			{
				if(player->optype & OP_ALLIN)
					return player->allin();
			}
		}
	}

	//当前小牌，跟注小于底注3倍
	if( player->optype & OP_CALL )
	{
		if( player->currMaxCoin < player->ante*4 )
		{
			return player->call();
		}
	}

	if(player->optype & OP_CHECK)
		return player->check();

	if(player->optype & OP_THROW)
		return player->throwcard();

	return 0;
}

int PlayerTimer::Round2Op()
{
	BYTE cbTempCard[5] = {0};
	cbTempCard[0] = player->card_array[0];
	cbTempCard[1] = player->card_array[1];
	cbTempCard[2] = player->CenterCard[0];
	cbTempCard[3] = player->CenterCard[1];
	cbTempCard[4] = player->CenterCard[2];
	short card1value = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short card2value = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	short cardcolor1 = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short cardcolor2 = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	if(card1value < card2value)
	{
		short temp = card1value;
		card1value = card2value;
		card2value = temp;
	}

	player->m_GameLogic.SortCardList( cbTempCard, 5 );
	BYTE bCardType = player->m_GameLogic.GetCardType(cbTempCard, 5);
	//_LOG_DEBUG_("cbTempCard:%02x, %02x, %02x, %02x, %02x\n",cbTempCard[0],cbTempCard[1], 
		//cbTempCard[2], cbTempCard[3], cbTempCard[4]);
	
	bool bLargest = player->robotKonwIsLargest();
	int randnum = rand()%100;

	bool isInHand = false;
	tagAnalyseResult AnalyseResult;
	player->m_GameLogic.AnalysebCardData(cbTempCard,5,AnalyseResult);
	if( AnalyseResult.cbThreeCount > 0 && 
		(AnalyseResult.cbThreeLogicVolue[0] == card1value || AnalyseResult.cbThreeLogicVolue[0] == card2value))
	{
		isInHand = true;
	}
	//三条以上
	if( bCardType > CT_THREE_TIAO || ( bCardType == CT_THREE_TIAO && isInHand ) )
	{
		//梭哈
		if( bCardType > CT_TONG_HUA )
		{
			if( randnum < 50)
			{
				if( player->optype & OP_ALLIN )
					return player->allin();
			}
		}
		//加注
		if( player->optype & OP_RASE )
		{
			if( player->setRandRaseCoin( 2, 8 ) == 0 )
			{
				if( player->optype & OP_ALLIN )
					return player->allin();
			}
			else
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				if( randnum < 80 )
					return player->rase();
			}
		}
		//跟注
		if( player->optype & OP_CALL )
			return player->call();
	}
	//对子、两对
	if( bCardType > CT_SINGLE )
	{
		bool isPaireInHand = false;
		for( int i = 0; i < AnalyseResult.cbLONGCount; i++ )
		{
			if( AnalyseResult.cbLONGLogicVolue[i] == card1value || 
				AnalyseResult.cbLONGLogicVolue[i] == card2value )
			{
				isPaireInHand = true;
				break;
			}
		}
		//底牌中有相关的对子
		if( isPaireInHand )
		{
			//加注
			if( player->optype & OP_RASE )
			{
				if( player->setRandRaseCoin( 2, 3 ) == 0 )
				{
					if( player->limitcoin < player->ante*6 )
					{
						if( player->optype & OP_ALLIN )
							return player->allin();
					}
				}
				else if( player->currMaxCoin <= player->ante*6 )
				{
					if(player->betCoinList[player->currRound] != 0)
					{
						if(player->optype & OP_CALL)
							return player->call();
					}
					if( randnum < 50 )
						return player->rase();
				}
			}

			if( player->currMaxCoin < player->ante*5 )
			{
				if( player->optype & OP_CALL )
					return player->call();
			}
		}
	}
	//这一轮，由此用户开始
	if( player->currMaxCoin == 0 && bCardType > CT_ONE_LONG )
	{
		if( randnum < 60 )
		{
			if(player->optype & OP_RASE)
			{
				if( player->setRandRaseCoin( 1, 3 ) == 1 )
					return player->rase();
			}
		}
	}

	if(switchmoney > switchmoneyup)
	{
		if(randnum < 40)
		{
			if(player->optype & OP_CALL)
				return player->call();
		}
		
		if(!bLargest)
		{
			int randrase = rand()%100;
			if(randrase < 60)
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				if((player->optype & OP_RASE))
				{
					if(player->setRandRaseCoin( 2, 8) > 0)
						return player->rase();
				}
			}
		}
		else
		{
			if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
			{
				if(player->currMaxCoin > player->ante*15)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
				if(player->optype & OP_CALL)
					return player->call();
			}
			int temprand = rand()%100;
			if(player->currMaxCoin <= player->ante*10)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
			else
			{
				if(temprand < 30)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
			}
		}
	}
	else
	{
		if(bLargest)
		{
			if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				int randrase = rand()%100;
				if(randrase < 60)
				{
					if((player->optype & OP_RASE))
					{
						if(player->setRandRaseCoin( 3, 5) > 0)
							return player->rase();
					}
				}
			}
			else
			{
				int randrase = rand()%100;
				if(randrase < 60 && player->currMaxCoin < player->ante*4)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
			}
		}
	}

	if(player->optype & OP_CHECK)
		return player->check();

	if(player->optype & OP_THROW)
		return player->throwcard();
	return 0;
}

int PlayerTimer::Round3Op()
{
	short card1value = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short card2value = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	short cardcolor1 = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short cardcolor2 = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	if(card1value < card2value)
	{
		short temp = card1value;
		card1value = card2value;
		card2value = temp;
	}
	bool bLargest = player->robotKonwIsLargest();
	int randnum = rand()%100;

	BYTE bCardType = player->finalcardvalue;
	randnum = rand()%100;
	bool isInHand = false;
	tagAnalyseResult AnalyseResult;
	player->m_GameLogic.AnalysebCardData(player->Knowcard_array,5,AnalyseResult);
	if( AnalyseResult.cbThreeCount > 0 && 
		(AnalyseResult.cbThreeLogicVolue[0] == card1value || AnalyseResult.cbThreeLogicVolue[0] == card2value))
	{
		isInHand = true;
	}
	//三条以上
	if( bCardType > CT_THREE_TIAO || ( bCardType == CT_THREE_TIAO && isInHand ) )
	{
		//梭哈
		if( bCardType > CT_TONG_HUA )
		{
			if( randnum < 20)
			{
				if( player->optype & OP_ALLIN )
					return player->allin();
			}
		}
		//加注
		if( player->optype & OP_RASE )
		{
			if( player->setRandRaseCoin(3, 6 ) == 0 )
			{
				if( player->optype & OP_ALLIN )
					return player->allin();
			}
			else
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				return player->rase();
			}
		}
		//跟注
		if( player->optype & OP_CALL )
			return player->call();
	}

	if(switchmoney > switchmoneyup)
	{
		if(randnum < 40)
		{
			if(player->optype & OP_CALL)
				return player->call();
		}
		
		if(!bLargest)
		{
			int randrase = rand()%100;
			if(randrase < 60)
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				if((player->optype & OP_RASE))
				{
					if(player->setRandRaseCoin( 3, 3) > 0)
						return player->rase();
				}
			}
		}
		else
		{
			if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
			{
				if(player->currMaxCoin > player->ante*15)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
				if(player->optype & OP_CALL)
					return player->call();
			}
			int temprand = rand()%100;
			if(player->currMaxCoin <= player->ante*10)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
			else
			{
				if(temprand < 30)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
			}
		}
	}
	else
	{
		if(bLargest)
		{
			if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				int randrase = rand()%100;
				if(randrase < 60)
				{
					if((player->optype & OP_RASE))
					{
						if(player->setRandRaseCoin( 4, 5) > 0)
							return player->rase();
					}
				}
			}
			else
			{
				int randrase = rand()%100;
				if(randrase < 60 && player->currMaxCoin < player->ante*4)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
			}
		}
	}


	if(player->optype & OP_CHECK)
		return player->check();

	if(player->optype & OP_THROW)
		return player->throwcard();
	return 0;
}

int PlayerTimer::Round4Op()
{
	short card1value = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short card2value = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	short cardcolor1 = player->m_GameLogic.GetCardLogicValue(player->card_array[0]);
	short cardcolor2 = player->m_GameLogic.GetCardLogicValue(player->card_array[1]);
	if(card1value < card2value)
	{
		short temp = card1value;
		card1value = card2value;
		card2value = temp;
	}


	BYTE bCardType = player->finalcardvalue;
	bool bLargest = player->robotKonwIsLargest();
	int randnum = rand()%100;

	
	if( bCardType > CT_HU_LU && player->robotKonwIsLargest() )
	{
		if( player->optype & OP_ALLIN )
			return player->allin();
	}
	bool isInHand = false;
	tagAnalyseResult AnalyseResult;
	player->m_GameLogic.AnalysebCardData(player->Knowcard_array,5,AnalyseResult);
	if( AnalyseResult.cbThreeCount > 0 && 
		(AnalyseResult.cbThreeLogicVolue[0] == card1value || AnalyseResult.cbThreeLogicVolue[0] == card2value))
	{
		isInHand = true;
	}
	//三条以上
	if( bCardType > CT_THREE_TIAO || ( bCardType == CT_THREE_TIAO && isInHand ) )
	{
		//梭哈
		if( randnum < 10)
		{
			if( player->optype & OP_ALLIN )
				return player->allin();
		}
		//加注
		if( player->optype & OP_RASE )
		{
			if( player->setRandRaseCoin( 4, 5 ) == 0 )
			{
				if( player->optype & OP_ALLIN )
					return player->allin();
			}
			else
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				return player->rase();
			}
		}
		//跟注
		if( player->optype & OP_CALL )
			return player->call();
	}

	if(switchmoney > switchmoneyup)
	{
		if(randnum < 20)
		{
			if(player->optype & OP_CALL)
				return player->call();
		}

		
		if(!bLargest)
		{
			int randrase = rand()%100;
			if(randrase < 60)
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				if((player->optype & OP_RASE))
				{
					if(player->setRandRaseCoin( 4, 2) > 0)
						return player->rase();
				}
			}
		}
		else
		{
			if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
			{
				if(player->currMaxCoin > player->ante*15)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
				if(player->optype & OP_CALL)
					return player->call();
			}
			int temprand = rand()%100;
			if(player->currMaxCoin <= player->ante*10)
			{
				if(player->optype & OP_CALL)
					return player->call();
			}
			else
			{
				if(temprand < 30)
				{
					if(player->optype & OP_ALLIN)
						return player->allin();
				}
			}
		}
	}
	else
	{
		if(bLargest)
		{
			if( (card1value > 10 && card2value > 10) || (cardcolor1 == cardcolor2 ) || card1value == 0x0E || card1value == 0x0E
				|| ((card1value>10 || card2value>10) && ((card1value - card2value) == 1)))
			{
				if(player->betCoinList[player->currRound] != 0)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
				int randrase = rand()%100;
				if(randrase < 60)
				{
					if((player->optype & OP_RASE))
					{
						if(player->setRandRaseCoin( 4, 5) > 0)
							return player->rase();
					}
				}
			}
			else
			{
				int randrase = rand()%100;
				if(randrase < 60 && player->currMaxCoin < player->ante*4)
				{
					if(player->optype & OP_CALL)
						return player->call();
				}
			}
		}
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
	player->logout();
	return 0;
}

int PlayerTimer::ActiveLeaveTimeOut()
{
	this->stopActiveLeaveTimer();
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
		//_LOG_ERROR_("HeatTimeOut player uid[%d] is not this uid[%d]\n", player->id, uid);
		return -1;
	}
	player->heartbeat();
	this->startHeartTimer(uid, 30);
	return 0;
}

int PlayerTimer::StartTimeOut()
{
	this->stopStartGameTimer();
	player->stargame();
	return 0;
}
