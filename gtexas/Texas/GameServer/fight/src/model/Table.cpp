#include <stdio.h>
#include <stdlib.h>
#include "Table.h"
#include "AllocSvrConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "IProcess.h"
#include "RedisOprs.h"
#include "CoinConf.h"
#include "GameCmd.h"

Table::Table():m_nStatus(-1)
{	 
}
Table::~Table()
{
}

void Table::init()
{
	timer.init(this);
	stopAllTimer();
	this->m_nStatus = STATUS_TABLE_EMPTY;
	m_nType = Configure::getInstance()->level;
	m_lBigBlind = 0;
	m_lTax = 0;
	memset(player_array, 0, sizeof(player_array));
	m_nCountPlayer = 0;
	memset(leaverarry, 0, sizeof(leaverarry));
	memset(m_cbCenterCardData, 0, sizeof(m_cbCenterCardData));
	leavercount=0;

	m_nOwner = 0;
	m_bHaspasswd = false;
	tableName[0] = '\0';
	password[0] = '\0';
	GameID[0] = '\0';
	EndType = 0;
	kicktimeflag = false;
	isthrowwin = false;
	m_nRePlayTime = 0;
	SendCardTime = 0;
	m_pFirstBetter = NULL;
	hassendcard = false;
	m_bMaxNum = Configure::getInstance()->numplayer > 0 ? Configure::getInstance()->numplayer : 1024;
	m_nDealerIndex = 0;
	m_nSmallBlindIndex = 0;
	m_nBigBlindUid = 0;
	m_lGameLimt = 0;
	m_bCurrRound = 0;
	memset(m_PoolArray, 0, sizeof(m_PoolArray));
	m_nPoolNum = 0;
	m_bThisRoundHasAllin = false;
	m_bIsOverTimer = false;
	m_lSumPool = 0;
	m_bIsFirstSetPool = false;
	m_nMagicCoin = 10;
	m_nMustBetCoin = 0;
}

void Table::reset()
{
	memset(m_cbCenterCardData, 0, sizeof(m_cbCenterCardData));
	memset(leaverarry, 0, sizeof(leaverarry));
	leavercount=0;
	GameID[0] = '\0';
	EndType = 0;
	kicktimeflag = false;
	hassendcard = false;
	m_bCurrRound = 0;
	m_pFirstBetter = NULL;
	m_nDealerIndex++;
	memset(m_PoolArray, 0, sizeof(m_PoolArray));
	m_nPoolNum = 0;
	m_bThisRoundHasAllin = false;
	m_bIsOverTimer = false;
	m_lSumPool = 0;
	m_bIsFirstSetPool = false;
	m_nMagicCoin = 10;
	this->setTableConf();
}

void Table::reSetInfo()
{
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = this->player_array[i];
		if(player)
			player->reset();
	}
	this->reset();
}

void Table::setTableConf()
{
	CoinCfg coincfg;
	coincfg.level = Configure::getInstance()->level;
	int ret = CoinConf::getInstance()->getCoinCfg(&coincfg);
	if (ret < 0)
	{
		LOGGER(E_LOG_DEBUG) << "get coin cfg error";
		return;
	}

	if (coincfg.tax > 100 || coincfg.tax <= 0)
		coincfg.tax = 5;
	this->m_lTax = float(coincfg.tax) * 0.01;
	this->m_lBigBlind    = coincfg.bigblind;
	this->m_nMagicCoin   = coincfg.magiccoin;
	this->m_nMustBetCoin = coincfg.mustbetcoin;
	
	LOGGER(E_LOG_DEBUG) << "m_lBigBlind=" << m_lBigBlind << " lTax=" << m_lTax << " m_nMagicCoin=" << m_nMagicCoin <<
		" m_nMustBetCoin=" << m_nMustBetCoin;
// 	int i = 0;
// 	short anteRate = 0;
// 	short texRate = 0;
// 	for(i = 0; i < m_bMaxNum; ++i)
// 	{
// 		Player* player = player_array[i];
// 		if(player)
// 		{
// 			m_lBigBlind = player->coincfg.bigblind;
// 			m_lTax = player->coincfg.tax;
// 			m_nMagicCoin = player->coincfg.magiccoin;
// 			m_nMustBetCoin = player->coincfg.mustbetcoin;
// 			break;
// 		}
// 	}

// 	if (Configure::getInstance()->level == PRIVATE_ROOM)
// 	{
// 		int64_t minmoney = 0x7FFFFFFFFFFFFF;
// 		short minrateante = 1000;
// 		short minratetax = 1000;
// 		for(i = 0; i < m_bMaxNum; ++i)
// 		{
// 			Player* player = player_array[i];
// 			if(player)
// 			{
// 				if(player->coincfg.rateante < minrateante)
// 					minrateante = player->coincfg.rateante;
// 				if(player->coincfg.ratetax < minratetax)
// 					minratetax = player->coincfg.ratetax;
// 				if(player->m_lCarryMoney < minmoney)
// 					minmoney = player->m_lCarryMoney;
// 			}
// 		}
// 		_LOG_DEBUG_("minrateante:%d minratetax:%d minmoney:%d\n", minrateante, minratetax, minmoney);
// 		if(((minmoney*minrateante)/100) < 1)
// 			this->m_lBigBlind = 1;
// 		else
// 			this->m_lBigBlind = (minmoney*minrateante)/100;
// 
// 		this->m_lTax = (minratetax*this->m_lBigBlind)/100 ;
// 	}
}

bool Table::isUserInTab(int uid)
{
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player && player->id == uid)
			return true;
	}
	return false;
}

Player* Table::getPlayer(int uid)
{
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player && player->id == uid)
			return player;
	}
	for (PlayerSet::iterator it = m_Viewers.begin(); it != m_Viewers.end(); it ++)
		if ((*it)->id == uid)
			return (*it);
	return NULL;
}

Player* Table::getPlayerInTab(int uid)
{
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player && player->id == uid)
			return player;
	}

	return NULL;
}

bool Table::isAllRobot()
{
	for(int i = 0; i < this->leavercount; ++i)
	{
		if(leaverarry[i].source != 30)
			return false;
	}

	for(int i = 0; i < m_bMaxNum; ++i)
	{
		if(player_array[i] && player_array[i]->source != 30)
		{
			//排除掉还没有准备的情况
			if(player_array[i]->isComming())
				continue;
			if(!player_array[i]->isReady())
				return false;
		}
	}
	return true;
}

int Table::playerComming(Player* player, int tabindex)
{
	if(player == NULL)
		return -1;
	//如果此用户已经在此桌子则把另外那个player清除，替代为新的player
	if(isUserInTab(player->id))
	{
		for(int i = 0; i < m_bMaxNum; ++i)
		{
			if(player_array[i] && player_array[i]->id == player->id)
			{
				player->m_nStatus = player_array[i]->m_nStatus;
				player->tid = player_array[i]->tid;
				player->m_nTabIndex = player_array[i]->m_nTabIndex;
				player->m_bThisRoundBet = player_array[i]->m_bThisRoundBet;
				player->m_bAllin = player_array[i]->m_bAllin;
				player->optype = player_array[i]->optype;
				player->m_bHasCard = player_array[i]->m_bHasCard;
				player->m_lCarryMoney = player_array[i]->m_lCarryMoney;
				player->betCoinList[0] = player_array[i]->betCoinList[0];
				player->betCoinList[1] = player_array[i]->betCoinList[1];
				player->betCoinList[2] = player_array[i]->betCoinList[2];
				player->betCoinList[3] = player_array[i]->betCoinList[3];
				player->betCoinList[4] = player_array[i]->betCoinList[4];
				player->card_array[0] = player_array[i]->card_array[0];
				player->card_array[1] = player_array[i]->card_array[1];
				player_array[i]->init();
				player_array[i] = player;
			}
		}
		return 0;
	}
	//说明此房间这个用户进入不成功，返回-2之后重新给此用户分配房间
	if(playerSeatInTab(player, tabindex) < 0)
		return -2;
	//如果桌子不是在玩牌则状态变为等待准备状态
	if(!this->isActive())
	{
		this->m_nStatus = STATUS_TABLE_READY;
	}
	//这里设置用户进入的标志，并且设置状态
	player->enter();
	//当前用户数加1
	++this->m_nCountPlayer;

	if(this->m_nCountPlayer == 1)
	{
		this->setTableConf();
	}
	AllocSvrConnect::getInstance()->updateTableUserCount(this);
	return 0;
}

void Table::playerLeave(int uid)
{
	Player* player = this->getPlayer(uid);
	if(player)
	{
		this->playerLeave(player);
	}
}

void Table::playerLeave(Player* player)
{
	if(player == NULL)
		return;
	if (!isUserInTab(player->id))
	{
		player->leave();
		LeaveViewers(player);
		return;
	}
	_LOG_WARN_("Tid[%d] Player[%d] Leave\n", this->id, player->id);
	this->setSeatNULL(player);
	player->leave();
	player->init();
	//如果桌子不是在玩牌则状态变为等待准备状态
	if(!this->isActive())
	{
		this->m_nStatus = STATUS_TABLE_READY;
	}
	//当前用户减一
	--this->m_nCountPlayer;
	//房间里面只剩一个人的时候直接解锁
	if(this->m_nCountPlayer == 1)
	{
		stopKickTimer();
		unlockTable();
	}

	if(this->isEmpty())
	{
		this->init();
	}
	AllocSvrConnect::getInstance()->updateTableUserCount(this);
}

void Table::setSeatNULL(Player* player)
{
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		if(player_array[i] == player)
			player_array[i] = NULL;
	}
}

int Table::playerSeatInTab(Player* complayer)
{
	_LOG_WARN_("TheMaxNum: %d", m_bMaxNum);
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		if(player_array[i] == NULL)
		{
			player_array[i] = complayer;
			complayer->m_nStatus = STATUS_PLAYER_COMING;
			complayer->tid = this->id;
			complayer->m_nTabIndex = i;
			_LOG_WARN_("TheFreeSeat: %d", i);
			return 0;
		}
	}
	return -1;
}

bool Table::isAllReady()
{
	int readyCount=0;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		if(player_array[i] && player_array[i]->isReady())
			readyCount++;
		if(readyCount == this->m_nCountPlayer)
			return true;
	}
	return false;
}


bool Table::isCanGameStart()
{
	int readyCount=0;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		if(player_array[i] && player_array[i]->isReady())
			readyCount++;
		//大于两个人准备则说明此盘游戏可以开始了
		if(readyCount >= 2)
			return true;
	}
	return false;
}

void Table::gameStart()
{
	stopTableStartTimer();
	stopKickTimer();
	unlockTable();
	int i = 0;
	int j = 0;

	BYTE cbRandCard[FULL_COUNT] = {0};
	m_GameLogic.RandCardList(cbRandCard,GAME_PLAYER*MAX_COUNT+MAX_CENTERCOUNT);
	//BYTE cbRandCard[FULL_COUNT] = {0x01, 0x0D, 0x15, 0x1c, 0x1A, 0x0B, 0x0C, 0x02, 0x03, 0x1c, 0x31};
	int wCardCount = 0;
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player)
		{
			memcpy(player->card_array,&cbRandCard[(wCardCount++)*MAX_COUNT], sizeof(BYTE)*MAX_COUNT);
			char cardbuff[64]={0};
			for(j = 0; j < MAX_COUNT; ++j)
			{
				sprintf(cardbuff+5*j, "0x%02x ",player->card_array[j]);
			}
			player->m_bHasCard = true;
			player->m_nStatus = STATUS_PLAYER_ACTIVE;
			_LOG_INFO_("[gameStart] tid=[%d] uid=[%d] cardarray:[%s] \n", this->id, player->id, cardbuff);
			AllocSvrConnect::getInstance()->userUpdateStatus(player, STATUS_PLAYER_ACTIVE);
			setPlayerOptype(player, 0);
		}
	}
	memcpy(m_cbCenterCardData,&cbRandCard[wCardCount*MAX_COUNT],CountArray(m_cbCenterCardData));
	char cardbuff[64]={0};
	for(j = 0; j < 5; ++j)
	{
		sprintf(cardbuff+5*j, "0x%02x ",m_cbCenterCardData[j]);
	}
	_LOG_INFO_("[gameStart] centercard:%s\n", cardbuff);

	
	//设置桌子的台费规则
	this->setTableConf();

	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player)
		{
			player->m_nEndCardType = m_GameLogic.FiveFromSeven(player->card_array,MAX_COUNT,m_cbCenterCardData,MAX_CENTERCOUNT,player->cbEndCardData,MAX_CENTERCOUNT);
			char cardbuff[64]={0};
			for(j = 0; j < 5; ++j)
			{
				sprintf(cardbuff+5*j, "0x%02x ",player->cbEndCardData[j]);
			}
			_LOG_INFO_("[gameStart] uid:%d cardarray:[%s] cbEndCardKind:[%d]\n", player->id, cardbuff, player->m_nEndCardType);
			
			if(player_array[i]->source != 30)
			{
				int randnum = rand()%100;
				int  CurTotal = RedisOprs::getPlayerCompleteCount(player_array[i]->id);
				if( CurTotal < Configure::getInstance()->curBeforeCount )
				{
					int randTemp = Configure::getInstance()->esayTaskProbability;
					if( randTemp < 100 )
					{
						randTemp += rand()%(100 - randTemp);
					}

					if( randnum < randTemp )
					{
						player_array[i]->m_pTask = TaskManager::getInstance()->getRandEsayTask();
					}
				}
				
				if(player_array[i]->m_pTask == NULL)
				{
					player_array[i]->m_pTask = TaskManager::getInstance()->getRandTask();
				}

				if(player_array[i]->m_pTask)
				{
					if(player_array[i]->m_pTask->ningothigh - player_array[i]->m_pTask->ningotlow < 0)
						player_array[i]->m_nGetRoll = player_array[i]->m_pTask->ningotlow;
					else
						player_array[i]->m_nGetRoll = player_array[i]->m_pTask->ningotlow + rand()%(player_array[i]->m_pTask->ningothigh - player_array[i]->m_pTask->ningotlow + 1);
					_LOG_INFO_("player[%d] get taskid[%ld] taskname[%s] roll[%d] level[%d]\n", player_array[i]->id, player_array[i]->m_pTask->taskid, player_array[i]->m_pTask->taskname, player_array[i]->m_nGetRoll, this->m_nType);
				}
			}

		}
	}

	m_lGameLimt = getSecondLargeCoin();
	m_lCanMaxBetCoin = m_lGameLimt;

	for(i = 0; i < m_bMaxNum; ++i)
	{
		short index = (m_nDealerIndex + i)%m_bMaxNum;
		Player* player = player_array[index];
		if(player)
		{
			m_nDealerIndex = index;
			m_nDealerUid = player->id;
			break;
		}
	}

	for(i = 1; i < m_bMaxNum; ++i)
	{
		short index = (m_nDealerIndex + i)%m_bMaxNum;
		Player* player = player_array[index];
		if(player)
		{
			m_nSmallBlindIndex = index;
			m_nSmallBlindUid = player->id;
			if(player->m_lCarryMoney <= m_lBigBlind/2)
			{
				player->betCoinList[1] = player->m_lCarryMoney;
				player->m_bAllin = true;
				m_bThisRoundHasAllin = true;
			}
			else
				player->betCoinList[1] = m_lBigBlind/2;
			player->betCoinList[0] += player->betCoinList[1];
			player->m_lCarryMoney -= player->betCoinList[1];
			this->m_nRaseUid = m_nSmallBlindUid;
			this->m_lCurrMax = player->betCoinList[1];
			_LOG_DEBUG_("player:%d smallblind betcount:%d carrycoin:%d\n",player->id, player->betCoinList[0], player->m_lCarryMoney);
			break;
		}
	}

	for(i = 1; i < m_bMaxNum; ++i)
	{
		short index = (m_nSmallBlindIndex + i)%m_bMaxNum;
		Player* player = player_array[index];
		if(player)
		{
			m_nBigBlindIndex = index;
			m_nBigBlindUid = player->id;
			if(player->m_lCarryMoney <= m_lBigBlind)
			{
				player->betCoinList[1] = player->m_lCarryMoney;
				player->m_bAllin = true;
				m_bThisRoundHasAllin = true;
			}
			else
				player->betCoinList[1] = m_lBigBlind;
			player->betCoinList[0] += player->betCoinList[1];
			player->m_lCarryMoney -= player->betCoinList[1];
			if(m_lCurrMax < player->betCoinList[1])
			{
				m_lCurrMax = player->betCoinList[1];
				this->m_nRaseUid = m_nBigBlindUid;
			}
			_LOG_DEBUG_("player:%d bigblind betcount:%d carrycoin:%d\n",player->id, player->betCoinList[0], player->m_lCarryMoney);
			break;
		}
	}

	
	if(m_nMustBetCoin > 0)
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* player = player_array[i];
			if(player)
			{
				if(player->m_lCarryMoney <= this->m_nMustBetCoin)
				{
					player->betCoinList[1] += player->m_lCarryMoney;
					player->betCoinList[0] += player->m_lCarryMoney;
					player->m_lCarryMoney = 0;
					player->m_bAllin = true;
					m_bThisRoundHasAllin = true;
				}
				else
				{
					player->betCoinList[1] += this->m_nMustBetCoin;
					player->betCoinList[0] += this->m_nMustBetCoin;
					player->m_lCarryMoney -= this->m_nMustBetCoin;
				}
				if(m_lCurrMax < player->betCoinList[1])
				{
					m_lCurrMax = player->betCoinList[1];
				}
			}
		}
	}
	
	this->setSumPool();

	_LOG_DEBUG_("m_lCurrMax:%ld m_nRaseUid:%d m_nMustBetCoin:%d \n", m_lCurrMax, m_nRaseUid, m_nMustBetCoin);
	this->m_nStatus = STATUS_TABLE_ACTIVE;
	this->setStartTime(time(NULL));
	time_t t;
	time(&t);
	char time_str[32]={0};
    struct tm* tp= localtime(&t);
    strftime(time_str,32,"%Y%m%d%H%M%S",tp);
    char gameId[64] ={0};
    short tlevel = this->m_nType;
    sprintf(gameId, "%s|%02d|%02d|%d", time_str,tlevel,this->m_nCountPlayer,this->m_nDealerUid);
    this->setGameID(gameId); 
	_LOG_INFO_("[gameStart] tid=[%d] m_lBigBlind[%d] gameid[%s]\n",this->id, this->m_lBigBlind, this->getGameID());
	AllocSvrConnect::getInstance()->updateTableStatus(this);

}

int64_t Table::getSecondLargeCoin()
{
	int64_t firstcoin, secondcoin;
	firstcoin = secondcoin = 0;
	int i;
	int maxindex = -1;
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player && player->isActive() && player->m_bHasCard)
		{
			if(player->m_lCarryMoney > firstcoin)
			{
				maxindex = i;
				firstcoin = player->m_lCarryMoney;
			}
		}
	}
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player && player->isActive() && player->m_bHasCard && i != maxindex)
		{
			if(player->m_lCarryMoney > secondcoin)
			{
				secondcoin = player->m_lCarryMoney;			
			}
		}
	}
	//if(this->m_bCurrRound == 1)
	//	secondcoin += m_lBigBlind;
	return secondcoin;
}

int Table::setFirstBetter()
{
	int i = 0;
	
	if(this->m_bCurrRound == 5)
	{
		m_pFirstBetter = NULL;
		return 0;
	}

	//有牌并且没有allin的人数小于等于一个人直接没有第一次下注的用户
	short hascardNum = 0;
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player *player = this->player_array[i];
		if(player && player->isActive() && !player->m_bAllin && player->m_bHasCard)
			hascardNum++;
	}

	if(hascardNum <= 1)
	{
		m_pFirstBetter = NULL;
		return 0;
	}

	if(this->m_bCurrRound == 1)
	{
		for(i = 1; i < m_bMaxNum; ++i)
		{
			short index = (m_nBigBlindIndex+i)%m_bMaxNum;
			Player *player = this->player_array[index];
			if(player && player->isActive() && !player->m_bAllin && player->m_bHasCard)
			{
				m_pFirstBetter = player;
				return 0;
			}
		}
	}
	else
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			short index = (m_nSmallBlindIndex+i)%m_bMaxNum;
			Player *player = this->player_array[index];
			if(player && player->isActive() && !player->m_bAllin && player->m_bHasCard)
			{
				m_pFirstBetter = player;
				return 0;
			}
		}
	}
	return 0;
}

void Table::setPlayerOptype(Player* opplayer, short playeroptype)
{
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player *player = this->player_array[i];
		if(player && player->isActive() && player->m_bHasCard)
		{
			if(player->betCoinList[this->m_bCurrRound] >= this->m_lCurrMax)
				player->optype = OP_RASE|OP_ALLIN|OP_CHECK|OP_THROW;
			else
				player->optype = OP_CALL|OP_RASE|OP_ALLIN|OP_THROW;
			
			//钱都不够跟注了
			if(player->m_lCarryMoney <= (this->m_lCurrMax - player->betCoinList[this->m_bCurrRound]))
				player->optype = OP_ALLIN|OP_THROW;

			if(player->m_bAllin || this->m_bCurrRound == 5)
				player->optype = 0;
		}
	}

	return;
}

void Table::setPlayerlimitcoin()
{
	_LOG_DEBUG_("=-----------------maxbetcoin:%ld\n", m_lCanMaxBetCoin);
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player *player = this->player_array[i];
		if(player && player->isActive() && player->m_bHasCard)
		{
			int64_t diffcoin = m_lCanMaxBetCoin - player->betCoinList[this->m_bCurrRound];
			if(diffcoin > player->m_lCarryMoney)
				player->m_lBetLimit = player->m_lCarryMoney;
			else
				player->m_lBetLimit = diffcoin;

			if(player->m_lBetLimit < 0)
				player->m_lBetLimit = 0;
		}
	}
}


bool Table::isOnlyOneOprate()
{
	if(!this->isActive())
		return false;
	short activeCount = 0;
	short nothandcount = 0;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if(player && player->isActive() && player->m_bHasCard && !player->m_bAllin)
		{
			if(this->m_lCurrMax > 0 && player->betCoinList[this->m_bCurrRound] <= this->m_lCurrMax)
				++activeCount;
			if(this->m_lCurrMax == 0)
				++nothandcount;
			if(activeCount > 0 || nothandcount > 1)
				return false;
		}
	}
	return true;
}

Player* Table::getNextBetPlayer(Player* player, short playeroptype)
{
	if(player == NULL)
		return NULL;

	//说明此用户已经有操作行为
	if(this->m_bCurrRound != 1)
		player->timeoutCount = 0;
	//当前这轮此用户已经下注
	player->m_bThisRoundBet = true;
	for(int i = 1; i < m_bMaxNum; ++i)
	{
		int player_index = (player->m_nTabIndex+i)%m_bMaxNum;
		Player* otherplayer = this->player_array[player_index];
		if(otherplayer && otherplayer->id != player->id && otherplayer->isActive() && otherplayer->m_bHasCard)
		{
			//当前已经轮到的用户就是加注用户
			if(otherplayer->id == m_nRaseUid)
				return NULL;

			if(isOnlyOneOprate())
				return NULL;
			//说明下一个用户已经ALLIN了
			if(otherplayer->m_bAllin)
				continue;

			//当前这轮都还没有下过注,并且没有allin
			if(!otherplayer->m_bThisRoundBet)
			{
				//设置当前用户为将要下注的用户
				m_nCurrBetUid = otherplayer->id;
				return otherplayer;
			}

			int nextplayerbetcoin = otherplayer->betCoinList[this->m_bCurrRound];
			int thisplayerbetcoin = player->betCoinList[this->m_bCurrRound];
			if((m_lCurrMax == 0)&&(nextplayerbetcoin == thisplayerbetcoin))
				return NULL;

			//解决加注的人离开游戏，这轮一直不结束的bug
			if(otherplayer->m_bThisRoundBet && player->m_bThisRoundBet)
			{
				if(nextplayerbetcoin == thisplayerbetcoin && m_lCurrMax == nextplayerbetcoin)
					return NULL;
			}
			
			//设置当前用户为将要下注的用户
			m_nCurrBetUid = otherplayer->id;
			return otherplayer;
		}
	}
	return NULL;
}

int Table::setNextRound()
{
	this->stopBetCoinTimer();

	if(!hassendcard)
	{
		if(m_bCurrRound == 1 && !m_bIsFirstSetPool)
		{
			setBottomPool(m_bCurrRound);
			this->m_bThisRoundHasAllin = false;
		}

		if(m_bCurrRound > 1)
		{
			setBottomPool(m_bCurrRound);
			this->m_bThisRoundHasAllin = false;
		}
		hassendcard = true;
		this->startSendCardTimer(1000);
		return 0;
	}
	hassendcard = false;
	this->m_bCurrRound++;

	m_pFirstBetter = NULL;
	//第一轮已经设置过了
	if(m_bCurrRound != 1)
	//当前这轮能下注的最大额度为多少
		m_lCanMaxBetCoin = getSecondLargeCoin();

	setFirstBetter();

	if(m_pFirstBetter)
	{
		this->m_nRaseUid = this->m_nCurrBetUid = m_pFirstBetter->id;
		this->startBetCoinTimer(m_pFirstBetter->id, Configure::getInstance()->betcointime);
		m_pFirstBetter->setBetCoinTime(time(NULL));
	}

	//当第一轮大小盲时期就不用再有操作下注行为就可以直接发牌结束的情况
	if(m_bCurrRound == 1 && m_pFirstBetter == NULL)
	{
		setBottomPool(m_bCurrRound);
		m_bIsFirstSetPool = true;
		this->m_bThisRoundHasAllin = false;
	}

	for(int i = 0; i < m_nPoolNum+1; ++i)
	{
		_LOG_DEBUG_("=========+++++++========i:%d count:%ld\n", i, m_PoolArray[i].betCoinCount);
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(m_PoolArray[i].userBet[j].uid != 0)
				_LOG_DEBUG_(" j:%d uid:%d betcoin:%ld \n",j, m_PoolArray[i].userBet[j].uid, m_PoolArray[i].userBet[j].betcoin);
		}
	}

	if(m_bCurrRound > 1)
		m_lCurrMax = 0;
	
	setPlayerlimitcoin();
	setPlayerOptype(NULL, 0);
	
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* getplayer = this->player_array[i];
		if(getplayer)
		{
			getplayer->m_bThisRoundBet = false;//发牌的时候把所用用户当前轮是否下注置为否
			IProcess::sendRoundCard(getplayer, this, this->m_bCurrRound);
		}
	}

	if(this->m_bCurrRound == 5)
		return gameOver();

	if(m_pFirstBetter == NULL)
		return setNextRound();
	return 0;
}

int Table::setBottomPool(short currRound)
{
	int i = 0;
	int j = 0;

	int64_t tempBetArray[9];
	short tempIndexArray[9];

	int count = 0;
	_LOG_DEBUG_("currRound:%d realround:%d hasallin:%s\n", currRound, this->m_bCurrRound, m_bThisRoundHasAllin ? "true" : "false");
	//这轮有人allin
	if(m_bThisRoundHasAllin)
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* getplayer = this->player_array[i];
			if(getplayer && getplayer->betCoinList[currRound] != 0)
			{
				tempBetArray[count] = getplayer->betCoinList[currRound];
				tempIndexArray[count] = i;
				count++;
			}
		}

		for(i = 0; i < leavercount; ++i)
		{
			if(leaverarry[i].betCoinList[currRound] != 0)
			{
				tempBetArray[count] = leaverarry[i].betCoinList[currRound];
				tempIndexArray[count] = i + 10;
				count++;
			}
		}
		printf("=============---------count:%d\n",count);

		if(count == 1)
		{
			m_PoolArray[m_nPoolNum].betCoinCount += tempBetArray[0];
			setUserBetBool(tempIndexArray[0], tempBetArray[0]);
			return 0;
		}

		for(i = 0; i < count; ++i)
		{
			for(j = i+1; j < count; ++j)
			{
				if(tempBetArray[i] > tempBetArray[j])
				{
					int64_t tempcoin = tempBetArray[i];
					tempBetArray[i] = tempBetArray[j];
					tempBetArray[j] = tempcoin;
					short tempi = tempIndexArray[i];
					tempIndexArray[i] = tempIndexArray[j];
					tempIndexArray[j] = tempi;
				}
			}
		}

		for(i = 0; i < count; ++i)
		{
			printf("==============i:%d betarray:%ld index:%d\n", i, tempBetArray[i], tempIndexArray[i]);
		}

		for(i = 0; i < count; ++i)
		{
			if(tempBetArray[i] > 0)
			{
				for(j = i+1; j < count; ++j)
				{
					if(tempBetArray[j] > 0)
					{
						m_PoolArray[m_nPoolNum].betCoinCount += tempBetArray[i];
						tempBetArray[j] -= tempBetArray[i];
						setUserBetBool(tempIndexArray[j], tempBetArray[i]);
					}
				}
				if(m_PoolArray[m_nPoolNum].betCoinCount == 0)
				{
					m_PoolArray[m_nPoolNum].betCoinCount += tempBetArray[i];
					setUserBetBool(tempIndexArray[i], tempBetArray[i]);
				}
				else
				{
					m_PoolArray[m_nPoolNum].betCoinCount += tempBetArray[i];
					setUserBetBool(tempIndexArray[i], tempBetArray[i]);
					if(tempIndexArray[i] < 10)
					{
						if(player_array[tempIndexArray[i]]->m_bHasCard)
							m_nPoolNum++;
					}
				}
				tempBetArray[i] = 0;
			}
		}
	}
	else
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* getplayer = this->player_array[i];
			if(getplayer && getplayer->betCoinList[currRound] != 0)
			{
				m_PoolArray[m_nPoolNum].betCoinCount += getplayer->betCoinList[currRound];
				setUserBetBool(i, getplayer->betCoinList[currRound]);
			}
		}

		for(i = 0; i < leavercount; ++i)
		{
			if(leaverarry[i].betCoinList[currRound] != 0)
			{
				m_PoolArray[m_nPoolNum].betCoinCount += leaverarry[i].betCoinList[currRound];
				setUserBetBool(i+10, leaverarry[i].betCoinList[currRound]);
			}
		}
	}
	return 0;
}

void Table::setUserBetBool(int index, int64_t betroundcoin)
{
	int tempuid = 0;
	if(index < 10)
		tempuid = player_array[index]->id;
	else
		tempuid = leaverarry[index - 10].uid;
	for(int m = 0; m < GAME_PLAYER; ++m)
	{
		if(m_PoolArray[m_nPoolNum].userBet[m].uid  == tempuid)
		{
			m_PoolArray[m_nPoolNum].userBet[m].betcoin += betroundcoin;
			return;
		}
	}

	short insertindex = 0;
	for(int m = 0; m < GAME_PLAYER; ++m)
	{
		if(m_PoolArray[m_nPoolNum].userBet[m].uid  == 0)
		{
			insertindex = m;
			break;
		}
	}
	m_PoolArray[m_nPoolNum].userBet[insertindex].uid = tempuid;
	m_PoolArray[m_nPoolNum].userBet[insertindex].betcoin += betroundcoin;
}

bool Table::SelectMaxUser(UserWinList &EndResult)
{
	//清理数据
	ZeroMemory(&EndResult,sizeof(EndResult));

	//First数据
	WORD wWinnerID;
	WORD i = 0;
	for (i=0;i<m_bMaxNum;i++)
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver() && getplayer->m_bHasCard)
		{
			if(getplayer->cbTempCardDate[0] != 0)
			{
				wWinnerID=i;
				break;
			}
		}
	}
	
	//过滤全零
	if(i==m_bMaxNum)return false;


	//查找最大用户
	WORD wTemp = wWinnerID;
	for(i=1;i<m_bMaxNum;i++)
	{
		WORD j=(i+wTemp)%m_bMaxNum;
		Player* getplayer = this->player_array[j];
		if(getplayer && getplayer->isGameOver() && getplayer->m_bHasCard)
		{
			if(getplayer->cbTempCardDate[0]==0)continue;
			if(m_GameLogic.CompareCard(getplayer->cbTempCardDate,player_array[wWinnerID]->cbTempCardDate,MAX_CENTERCOUNT)>1)
			{
				wWinnerID=j;
			}
		}
	}

	//查找相同数据
	EndResult.wWinerList[EndResult.bSameCount++] = wWinnerID;
	for(i=0;i<m_bMaxNum;i++)
	{
		if(i==wWinnerID)continue;
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver() && getplayer->m_bHasCard)
		{
			if(getplayer->cbTempCardDate[0]==0) continue;
			if(m_GameLogic.CompareCard(getplayer->cbTempCardDate,player_array[wWinnerID]->cbTempCardDate,MAX_CENTERCOUNT)==FALSE)
			{
				EndResult.wWinerList[EndResult.bSameCount++] = i;
			}
		}
	}
	
	//从小到大
	if(EndResult.bSameCount>1)
	{
		int iSameCount=(int)EndResult.bSameCount;
		while((iSameCount--)>0)
		{
			int64_t lTempSocre = player_array[EndResult.wWinerList[iSameCount]]->betCoinList[0];
			for(int i=iSameCount-1;i>=0;i--)
			{
				if(lTempSocre < player_array[EndResult.wWinerList[i]]->betCoinList[0])
				{
					lTempSocre = player_array[EndResult.wWinerList[i]]->betCoinList[0];
					WORD wTemp = EndResult.wWinerList[iSameCount];
					EndResult.wWinerList[iSameCount] = EndResult.wWinerList[i];
					EndResult.wWinerList[i] = wTemp;
				}
			}
		}
	}

	return true;
}

int Table::calcCoin()
{
	int i = 0;
	short hasCardNum = 0;
	Player* onePlayer = NULL;
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver() && getplayer->m_bHasCard)
		{
			_LOG_DEBUG_("cbTempCardDate------------- i:%d uid:%d \n", i, getplayer->id);
			memcpy(getplayer->cbTempCardDate, getplayer->cbEndCardData, MAX_CENTERCOUNT);
			onePlayer = getplayer;
			hasCardNum++;
		}
	}

	//把下注了的用户下注的钱加回到带入金币，方便后面计算用户真正输赢金币
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver() && getplayer->betCoinList[0] != 0)
		{	
			getplayer->m_lCarryMoney += getplayer->betCoinList[0];
		}
	}

	//只有一个人有牌
	if(hasCardNum == 1)
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* getplayer = this->player_array[i];
			if(getplayer && onePlayer->id != getplayer->id && getplayer->isGameOver() && getplayer->betCoinList[0] != 0)
			{
				onePlayer->m_lFinallGetCoin += getplayer->betCoinList[0];
				getplayer->m_lFinallGetCoin -= getplayer->betCoinList[0];
				setWinnerPool(onePlayer, getplayer->id, getplayer->betCoinList[0]);
				getplayer->betCoinList[0] = 0;
			}
		}
		for(i = 0; i < leavercount; ++i)
		{
			//修改的稳定版本到线上运行很久了，但是现在要处理用户逃跑的情况，改为后面的情况
			/*int64_t lMinScore = MIN(onePlayer->betCoinList[0], leaverarry[i].betcoin);
			onePlayer->m_lFinallGetCoin += lMinScore; 
			setWinnerPool(onePlayer, leaverarry[i].uid, lMinScore);*/
			onePlayer->m_lFinallGetCoin += leaverarry[i].betcoin;
			setWinnerPool(onePlayer, leaverarry[i].uid, leaverarry[i].betcoin);
		}
		printf("==========onePlayer:%d m_lFinallGetCoin:%ld poolnum:%d ", onePlayer->id, onePlayer->m_lFinallGetCoin, (int)onePlayer->m_setPoolResult.size());
		//for(int m = 0; m < onePlayer->m_setPoolResult.size(); ++m)
		for (std::set<int>::iterator it=onePlayer->m_setPoolResult.begin(); it!=onePlayer->m_setPoolResult.end(); ++it)
		{
			printf(" %d",*it);
		}
		printf("\n");
		return 0;
	}

	//把用户的赢顺序排列出来
	int wWinCount=0;
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver())
		{
			//查找最大用户
			if(this->SelectMaxUser(getplayer->tempWinlist))
			{
				wWinCount=i;
			}

			//删除胜利数据
			for (int j=0;j<getplayer->tempWinlist.bSameCount;j++)
			{
				ZeroMemory(player_array[getplayer->tempWinlist.wWinerList[j]]->cbTempCardDate,sizeof(BYTE)*MAX_CENTERCOUNT);
				_LOG_DEBUG_("i:%d j:%d wRemoveId:%d removeuid:%d getplayer:%d getplayer->cbTempCardDate:0x%02x wWinCount:%d\n", 
					i, j, getplayer->tempWinlist.wWinerList[j], player_array[getplayer->tempWinlist.wWinerList[j]]->id, getplayer->id, getplayer->cbTempCardDate[0], wWinCount);
			}
		}
	}

	//确定全输的用户所在的Index
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* getplayer = this->player_array[i];
		if(i >= wWinCount && getplayer && getplayer->isGameOver())
		{
			if(getplayer->tempWinlist.bSameCount == 0)
			{
				wWinCount = i;
				break;
			}
		}
	}

	//把弃牌的玩家加入结算行列
	for (i=0;i<m_bMaxNum;i++)     
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver() && !getplayer->m_bHasCard && getplayer->betCoinList[0] != 0)
		{
			player_array[wWinCount]->tempWinlist.wWinerList[player_array[wWinCount]->tempWinlist.bSameCount++] = i;
		}
	} 
	//把离开的用户加入结算行列
	for (i=0;i<this->leavercount;i++)     
	{
		player_array[wWinCount]->tempWinlist.wWinerList[player_array[wWinCount]->tempWinlist.bSameCount++] = i + 10;
	} 

	int winnerorderid[9] = {0};
	int winnercount = 0;
	for (i=0;i<m_bMaxNum;i++)     
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver())
		{
			//胜利人数                      
			int iWinCount = (int)getplayer->tempWinlist.bSameCount;
			_LOG_DEBUG_("+++++getplayer:%d betcoin:%ld iWinCount:%d\n", getplayer->id, getplayer->betCoinList[0], iWinCount);
			for(int j = 0; j < iWinCount; ++j)
			{
				if(getplayer->tempWinlist.wWinerList[j] < 10)
					winnerorderid[winnercount++] = player_array[getplayer->tempWinlist.wWinerList[j]]->id;
				//else
				//	winnerorderid[winnercount++] = leaverarry[getplayer->tempWinlist.wWinerList[j]].uid;
				_LOG_DEBUG_("i:%d iWinCount:%d WinnerList[i].wWinerList[j]:%d \n", i, iWinCount, getplayer->tempWinlist.wWinerList[j]);
			}
		}
	}   

	//得分情况  一定是按照牌型从大到小排列的情况处理
	for (i=0;i<m_bMaxNum-1;i++)
	{
		Player* winner = this->player_array[i];
		if(winner && winner->isGameOver())
		{
			//胜利人数
			int iWinCount = (int)(winner->tempWinlist.bSameCount);
			_LOG_DEBUG_("i:%d winner:%d iWinCount:%d\n", i, winner->id, iWinCount);
			if(0 == iWinCount)break;

			//胜利用户得分情况
			for(int j=0;j<iWinCount;j++)
			{
				//离开的用户不可能作为赢家
				if(winner->tempWinlist.wWinerList[j] >= 10)
					break;
				if(0 == player_array[winner->tempWinlist.wWinerList[j]]->betCoinList[0])continue;
				if(j>0&& player_array[winner->tempWinlist.wWinerList[j]]->betCoinList[0] - player_array[winner->tempWinlist.wWinerList[j-1]]->betCoinList[0] == 0)continue;
				
				_LOG_DEBUG_("winner player_array[winner->tempWinlist.wWinerList[j]]->id:%d\n",player_array[winner->tempWinlist.wWinerList[j]]->id);	
				
				//失败用户失分情况
				for(int k=i+1;k<m_bMaxNum;k++)
				{
					Player* loser = this->player_array[k];
					if(loser && loser->isGameOver())
					{
						_LOG_DEBUG_("loser:%d sameCount:%d\n", loser->id, loser->tempWinlist.bSameCount);
						//失败人数
						if(0 == loser->tempWinlist.bSameCount)break;

						for(int l=0;l<loser->tempWinlist.bSameCount;l++)
						{
							if(loser->tempWinlist.wWinerList[l] < 10)
							{
								WORD wLostId=loser->tempWinlist.wWinerList[l];
								WORD wWinId=winner->tempWinlist.wWinerList[j];

								_LOG_DEBUG_("wLostId:%d loserid:%d wWinId:%d winnerid:%d\n",
									wLostId, player_array[wLostId]->id, wWinId, player_array[wWinId]->id);
								//用户已赔空
								if(0 == player_array[loser->tempWinlist.wWinerList[l]]->betCoinList[0])continue;

								int64_t lMinScore = 0;

								//上家得分数目 winner->tempWinlist.wWinerList[j-1] 这个一定会小于10
								int64_t lLastScore = ((j>0)?player_array[winner->tempWinlist.wWinerList[j-1]]->betCoinList[0]:0);
								//把余下的赢的数额补上
								lMinScore = MIN(player_array[wWinId]->betCoinList[0]-lLastScore,player_array[wLostId]->betCoinList[0]);

								for(int m=j;m<iWinCount;m++)
								{
									//得分数目
									player_array[winner->tempWinlist.wWinerList[m]]->m_lFinallGetCoin +=lMinScore/(iWinCount-j);
									setWinnerPool(player_array[winner->tempWinlist.wWinerList[m]], player_array[wLostId]->id, lMinScore/(iWinCount-j), iWinCount-j);
									_LOG_DEBUG_("m:%d winner->tempWinlist.wWinerList[m]:%d lMinScore:%d iWinCount-j:%d\n", m, winner->tempWinlist.wWinerList[m], lMinScore, iWinCount-j);
								}

								//赔偿数目
								player_array[wLostId]->betCoinList[0] -=lMinScore;
								player_array[wLostId]->m_lFinallGetCoin -=lMinScore;
							}
							//大于等于10说明是逃跑的用户结算
							else
							{
								//用户已赔空
								if(0 == leaverarry[loser->tempWinlist.wWinerList[l] - 10].betcoin)continue;

								WORD wLostId=loser->tempWinlist.wWinerList[l] - 10;
								WORD wWinId=winner->tempWinlist.wWinerList[j];
								int64_t lMinScore = 0;

								_LOG_DEBUG_("----wLostId:%d wWinId:%d\n", wLostId, wWinId);
								//上家得分数目
								int64_t lLastScore = ((j>0)?player_array[winner->tempWinlist.wWinerList[j-1]]->betCoinList[0]:0);
								//把余下的赢的数额补上
								lMinScore = MIN(player_array[wWinId]->betCoinList[0]-lLastScore,leaverarry[wLostId].betcoin);

								for(int m=j;m<iWinCount;m++)
								{
									//得分数目
									player_array[winner->tempWinlist.wWinerList[m]]->m_lFinallGetCoin +=lMinScore/(iWinCount-j);
									setWinnerPool(player_array[winner->tempWinlist.wWinerList[m]], leaverarry[wLostId].uid, lMinScore/(iWinCount-j), iWinCount-j);
									_LOG_DEBUG_("---winner->tempWinlist.wWinerList[m]:%d lMinScore:%d iWinCount-j:%d\n", winner->tempWinlist.wWinerList[m], lMinScore, iWinCount-j);
								}

								//赔偿数目
								leaverarry[wLostId].betcoin -=lMinScore;
							}
						}
					}
				}
			}
		}
	}

	for(int i = 0; i < m_nPoolNum+1; ++i)
	{
		_LOG_DEBUG_("=====before Calc====+++++++========i:%d count:%ld\n", i, m_PoolArray[i].betCoinCount);
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(m_PoolArray[i].userBet[j].uid != 0)
				_LOG_DEBUG_(" j:%d uid:%d betcoin:%ld \n",j, m_PoolArray[i].userBet[j].uid, m_PoolArray[i].userBet[j].betcoin);
		}
	}

	//处理分配下注多出一个池子没有处理的情况
	for (i=0; i<winnercount; i++)
	{
		for (int j=0;j<m_bMaxNum; j++)
		{
			Player* winner = this->player_array[j];
			if(winner && winner->id == winnerorderid[i])
			{
				for(int m = 0; m < this->m_nPoolNum + 1; ++m)
				{
					if(m_PoolArray[m].betCoinCount > 0)
					{
						for(int n = 0; n < GAME_PLAYER; ++n)
						{
							if(m_PoolArray[m].userBet[n].uid == winner->id)
							{
								m_PoolArray[m].betCoinCount -= m_PoolArray[m].userBet[n].betcoin;
								winner->m_PoolNumArray[m] += m_PoolArray[m].userBet[n].betcoin;
								m_PoolArray[m].userBet[n].betcoin = 0;
								winner->m_setPoolResult.insert(m);
							}
						}
					}
				}
				break;
			}
		}
	}
	return 0;
}

void Table::setWinnerPool(Player* winner, int loserID, int64_t losecoin, short averageNum)
{
	if(winner == NULL || losecoin <= 0)
		return ;
	int i,j,m;
	i = j = m = 0;
	for(i = 0; i < this->m_nPoolNum + 1; ++i)
	{
		if(m_PoolArray[i].betCoinCount > 0)
		{
			for(j = 0; j < GAME_PLAYER; ++j)
			{
				if(m_PoolArray[i].userBet[j].uid == winner->id)
				{
					for(m = 0; m < GAME_PLAYER; ++m)
					{
						if(m_PoolArray[i].userBet[m].uid == loserID)
						{
							if(m_PoolArray[i].userBet[j].betcoin > 0)
							{
								_LOG_DEBUG_("loserID:%d betcoin:%d\n", loserID, m_PoolArray[i].userBet[j].betcoin);
								m_PoolArray[i].betCoinCount -= m_PoolArray[i].userBet[j].betcoin;
								winner->m_PoolNumArray[i] += m_PoolArray[i].userBet[j].betcoin;
								m_PoolArray[i].userBet[j].betcoin = 0;
							}
							int64_t tempSubCoin = m_PoolArray[i].userBet[m].betcoin/averageNum;
							short tempmod = m_PoolArray[i].userBet[m].betcoin % averageNum;
							if(!m_PoolArray[i].userBet[m].hashandle)
							{
								m_PoolArray[i].userBet[m].hashandle = true;
								tempSubCoin += tempmod;
							}
							_LOG_DEBUG_("loserID:%d tempSubCoin:%d\n", loserID, tempSubCoin);
							m_PoolArray[i].betCoinCount -= tempSubCoin;
							winner->m_PoolNumArray[i] += tempSubCoin;
							losecoin -= tempSubCoin;
							winner->m_setPoolResult.insert(i);
							if (losecoin <= 0)
								return;
							break;
						}
					}
					break;
				}
			}
		}
	}
}

bool Table::iscanGameOver()
{
	if(!this->isActive())
		return false;
	short activeCount = 0;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		//此人必须是active状态，并且有手牌
		if(player && player->isActive() && player->m_bHasCard)
		{
			++activeCount;
			if(activeCount > 1)
				return false;
		}
	}
	return true;
}

int Table::gameOver()
{
	this->stopBetCoinTimer();
	this->stopSendCardTimer();
	//设置上一盘赢牌的用户
	int i = 0;

	//如果是只有一个人有牌结束的情况直接算公共池
	if(iscanGameOver())
	{
		setBottomPool(this->m_bCurrRound);
		//IProcess::sendPoolinfo(this);
		for(i = 0; i < m_nPoolNum+1; ++i)
		{
			_LOG_DEBUG_("=========+++++++========i:%d count:%ld\n", i, m_PoolArray[i].betCoinCount);
			for(int j = 0; j < GAME_PLAYER; ++j)
			{
				if(m_PoolArray[i].userBet[j].uid != 0)
					_LOG_DEBUG_(" j:%d uid:%d betcoin:%ld \n",j, m_PoolArray[i].userBet[j].uid, m_PoolArray[i].userBet[j].betcoin);
			}
		}
	}

	/*for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = this->player_array[i];
		if(player && player->isActive())
		{
			player->m_nStatus = STATUS_PLAYER_OVER;
			AllocSvrConnect::getInstance()->userUpdateStatus(player, STATUS_PLAYER_OVER);
			player->startnum++;
			//结束的时候把此用户最终手牌类型算出来
			if(player->m_bHasCard)
			{
				//只有真的是走到第五轮的时候才会计算此用户的最终牌型
				if(this->m_bCurrRound != 5)
					player->m_nEndCardType = -1;
			}
		}
	}
	this->m_nStatus = STATUS_TABLE_OVER;
	this->setEndTime(time(NULL));

	AllocSvrConnect::getInstance()->updateTableStatus(this);
	this->calcCoin();*/

	_LOG_INFO_("[gameOver] gameid[%s]\n", this->getGameID());
	this->startGameOverTimer(GAME_OVER_TIME);
	return 0;
}

void Table::setKickTimer()
{
	if(this->isActive() || this->m_nCountPlayer <= 1)
		return;
	this->startKickTimer(Configure::getInstance()->kicktime);
	this->lockTable();
	/*for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = this->player_array[i];
		if(player && (!player->isActive() || !player->isReady())&& player->startnum == 1)
		{
			IProcess::serverWarnPlayerKick(this, player, Configure::getInstance()->kicktime - 2);
		}
	}*/
}

void Table::setSumPool()
{
	m_lSumPool = 0;
	int i = 0;
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = this->player_array[i];
		if(player && player->betCoinList[0] != 0)
		{
			m_lSumPool += player->betCoinList[0];
		}
	}
	//把离开的用户加入结算行列
	for (i=0;i<this->leavercount;i++)     
	{
		m_lSumPool += leaverarry[i].betCoinList[0];
	} 
}

int64_t Table::getSumPool()
{
	return m_lSumPool;
}

void Table::calcMaxValue(Player* player)
{
	if(player == NULL)
		return;
	short temp = player->m_nEndCardType;
	if(temp == -1)
		return;
	if(player->m_lMaxCardValue > 0)
	{
		BYTE szCardBuff[5] = {0};
		szCardBuff[0] = (player->m_lMaxCardValue & 0x0FF00000000) >> 32;
		szCardBuff[1] = (player->m_lMaxCardValue & 0x000FF000000) >> 24;
		szCardBuff[2] = (player->m_lMaxCardValue & 0x00000FF0000) >> 16;
		szCardBuff[3] = (player->m_lMaxCardValue & 0x0000000FF00) >> 8;
		szCardBuff[4] = (player->m_lMaxCardValue & 0x000000000FF);
		if(this->m_GameLogic.CompareCard(player->cbEndCardData, szCardBuff, 5) == 2)
		{
			int64_t temp0 = player->cbEndCardData[0];
			int64_t temp1 = player->cbEndCardData[1];
			int64_t temp2 = player->cbEndCardData[2];
			int64_t temp3 = player->cbEndCardData[3];
			int64_t temp4 = player->cbEndCardData[4]; 
			player->m_lMaxCardValue = temp0 << 32 | temp1 << 24 | temp2 << 16 | temp3 << 8 | temp4 ; 
		}
	}
	else
	{
		int64_t temp0 = player->cbEndCardData[0];
		int64_t temp1 = player->cbEndCardData[1];
		int64_t temp2 = player->cbEndCardData[2];
		int64_t temp3 = player->cbEndCardData[3];
		int64_t temp4 = player->cbEndCardData[4]; 
		player->m_lMaxCardValue = temp0 << 32 | temp1 << 24 | temp2 << 16 | temp3 << 8 | temp4 ; 
	}
}

void Table::privateRoomOp()
{
	if(this->m_nType == PRIVATE_ROOM)
	{
		for(int i = 0; i < m_bMaxNum; ++i)
		{
			Player* getplayer = this->player_array[i];
			if(getplayer)
			{
				_LOG_DEBUG_("player:%d betcoin:%ld gamelimit:%ld\n", getplayer->id, getplayer->betCoinList[0], this->m_lGameLimt);
				if(getplayer->betCoinList[0] == this->m_lGameLimt)
				{
					getplayer->optype = OP_THROW|OP_CHECK;
					IProcess::pushPrivateOprate(this);
				}
			}
		}
	}
}

//================时间操作函数=========================

void Table::stopAllTimer()
{
	this->timer.stopAllTimer();
}

void Table::startBetCoinTimer(int uid,int timeout)
{
	_LOG_DEBUG_("=====uid:%d====[startBetCoinTimer]\n", uid);
	timer.startBetCoinTimer(uid,timeout);
}

void Table::stopBetCoinTimer()
{
	_LOG_DEBUG_("=========[stopBetCoinTimer]\n");
	timer.stopBetCoinTimer();
}

void Table::startTableStartTimer(int timeout)
{
	_LOG_DEBUG_("=========[startTableStartTimer]\n");
	timer.startTableStartTimer(timeout);
}

void Table::stopTableStartTimer()
{
	_LOG_DEBUG_("=========[stopTableStartTimer]\n");
	timer.stopTableStartTimer();
}

void Table::startKickTimer(int timeout)
{
	_LOG_DEBUG_("=========[startKickTimer]\n");
	timer.startKickTimer(timeout);
}

void Table::stopKickTimer()
{
	_LOG_DEBUG_("=========[stopKickTimer]\n");
	timer.stopKickTimer();
}

void Table::startSendCardTimer(int timeout)
{
	_LOG_DEBUG_("=========[startSendCardTimer]\n");
	timer.startSendCardTimer(timeout);
}

void Table::stopSendCardTimer()
{
	_LOG_DEBUG_("=========[stopSendCardTimer]\n");
	timer.stopSendCardTimer();
}

void Table::startGameOverTimer(int timeout)
{
	_LOG_DEBUG_("=========[startGameOverTimer]\n");
	m_bIsOverTimer = true;
	timer.startGameOverTimer(timeout);
}

void Table::stopGameOverTimer()
{
	_LOG_DEBUG_("=========[stopGameOverTimer]\n");
	timer.stopGameOverTimer();
}


int Table::playerSeatInTab(Player* complayer, int tabindex)
{
	if (tabindex == -1) {
		for (int i = 0; i < GAME_PLAYER; i++) {
			if(player_array[i] == NULL)
			{
				player_array[i] = complayer;
				complayer->m_nStatus = STATUS_PLAYER_COMING;
				complayer->tid = this->id;
				complayer->m_nTabIndex = i;
				LeaveViewers(complayer);
				return 0;
			}
		}
	} else {
		if(player_array[tabindex] == NULL)
		{
			player_array[tabindex] = complayer;
			complayer->m_nStatus = STATUS_PLAYER_COMING;
			complayer->tid = this->id;
			complayer->m_nTabIndex = tabindex;
			LeaveViewers(complayer);
			return 0;
		}
	}

	return -1;
}

int Table::playerStandUp(Player* player)
{
	if (player->m_nTabIndex == -1)
		return 0;

	if (isUserInTab(player->id)) {
		m_nCountPlayer--;
		setSeatNULL(player);
		JoinViewers(player);
	}

	return 0;
}

int Table::playerSeatDown(Player* player, int tabindex)
{
	if (tabindex < 0 || tabindex >= GAME_PLAYER)
		return -38;

	if (player_array[tabindex] != NULL && player_array[tabindex]->id != player->id) {
		return -37;
	}

	if(isUserInTab(player->id)) {
		for (int i = 0; i < GAME_PLAYER; ++i) {
			if (player_array[i] && player_array[i]->id == player->id && tabindex != i) {
				playerStandUp(player_array[i]);
				break;
			}
		}
	}

	return playerComming(player, tabindex);
}

int Table::playerChangeSeat(Player* player, int tabindex)
{
	if (tabindex < 0 || tabindex >= GAME_PLAYER)
		return -38;

	if (player_array[tabindex] != NULL && player_array[tabindex]->id != player->id) {
		return -37;
	}

	playerStandUp(player);
	return playerSeatDown(player, tabindex);
}

void Table::LeaveViewers(Player* player)
{
	if (isViewers(player))
    	ULOGGER(E_LOG_DEBUG, player->id) << "LeaveViewers";
    m_Viewers.erase(player);
}

void Table::JoinViewers(Player* player)
{
	if (isViewers(player))
    	ULOGGER(E_LOG_DEBUG, player->id) << "JoinViewers";
    m_Viewers.insert(player);
}

void Table::SendGift(Player* player, int price, BYTE sendtoall)
{
    player->m_nMagicCoinCount = 0;
    if (sendtoall) {
        for (int i = 0; i < GAME_PLAYER; i++) {
            if (player_array[i] && player_array[i]->id != player->id) {
                player->m_lMoney -= price;
                player->m_nMagicCoinCount += price;
            }
        }
    } else {
        player->m_lMoney -= price;
        player->m_nMagicCoinCount += price;
    }

    IProcess::updateDB_UserCoin(player, this, 0);
    player->m_nMagicCoinCount = 0;

	IProcess::sendUpdateMoney(this, player);
}