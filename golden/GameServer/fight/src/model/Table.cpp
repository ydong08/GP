#include <stdio.h>
#include <stdlib.h>
#include "Table.h"
#include "AllocSvrConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "IProcess.h"
#include "CoinConf.h"
#include "GameUtil.h"
#include "HallManager.h"
#include "Logger.h"

using namespace std;

// 设置输家此局游戏金币数量的变化
// checkReturn - 表示是否需要返还
static void set_loser_coin(Table *pTable, Player *pWinner, Player *pLoser, bool checkReturn = false)
{
	pLoser->setFinalGetCoin(0 - pLoser->m_lSumBetCoin);

	// 输钱的人是暗牌，赢钱的人是明牌，并且都全下了，则输钱的人只输Allin的一半
	if (checkReturn &&
		pWinner->m_bCardStatus == CARD_KNOWN && pLoser->m_bCardStatus == CARD_UNKNOWN && pLoser->m_bAllin)
	{
		int64_t return_coin = pLoser->m_AllinCoin / 2;

		pLoser->setFinalGetCoin(return_coin);
		pTable->m_lSumPool -= return_coin;
	}
}

Table::Table():m_nStatus(-1), m_BetMaxCardStatus(CARD_UNKNOWN), m_nRaseUid(0), m_AllinCardStatus(CARD_DISCARD)
{	 
}

Table::Table(int _id) : m_nStatus(-1), m_BetMaxCardStatus(CARD_UNKNOWN), m_nRaseUid(0), m_AllinCardStatus(CARD_DISCARD), id(_id)
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
	m_nType = Configure::getInstance()->m_nLevel;
	m_nTax = 0;
	memset(player_array, 0, sizeof(player_array));
	m_nCountPlayer = 0;
	memset(leaverarry, 0, sizeof(leaverarry));
	memset(m_bTableCardArray, 0, sizeof(m_bTableCardArray));
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
	hassendcard = false;
	m_bMaxNum = Configure::getInstance()->numplayer;
	m_bPrivateMaxNum = Configure::getInstance()->numplayer;
	m_bCurrRound = 0;
	m_bIsOverTimer = false;
	m_nMagicCoin = 10;
	m_lCanMaxBetCoin = 0;
	m_nFirstIndex = 0;
	m_nPreWinIndex = -1;
	m_nDealerIndex = 0;
	m_nAnte = 0;
	m_nMaxLimit = 0;
	m_lMaxAllIn = 0;
	m_vecBetCoin.clear();
	m_lSumPool = 0;
	m_lCurrBetMax = 0;
	m_GameLogic.CoppyCardList();
	m_lLeaveSumCoin = 0;
	m_nRase1 = 0;
	m_nRase2 = 0;
	m_nRase3 = 0;
	m_nRase4 = 0;
	m_bSetAllinCoinFlag = false;
	m_lHasAllInCoin = 0;

	m_nMulCount = Configure::getInstance()->mulcount;
	m_nMulNum1 = Configure::getInstance()->mulnum1;
	m_nMulCoin1 = Configure::getInstance()->mulcoin1;
	m_nMulNum2 = Configure::getInstance()->mulnum2;
	m_nMulCoin2 = Configure::getInstance()->mulcoin2;

	m_nForbidCount = Configure::getInstance()->forbidcount;
	m_nForbidNum1 = Configure::getInstance()->forbidnum1;
	m_nForbidCoin1 = Configure::getInstance()->forbidcoin1;
	m_nForbidNum2 = Configure::getInstance()->forbidnum2;
	m_nForbidCoin2 = Configure::getInstance()->forbidcoin2;

	m_nChangeCount = Configure::getInstance()->changecount;
	m_nChangeCoin1 = Configure::getInstance()->changecoin1;
	m_nChangeCoin2 = Configure::getInstance()->changecoin2;
	m_nChangeCoin3 = Configure::getInstance()->changecoin3;

	m_nRaseUid = 0;
}

void Table::reset()
{
	memset(m_bTableCardArray, 0, sizeof(m_bTableCardArray));
	memset(leaverarry, 0, sizeof(leaverarry));
	leavercount=0;
	GameID[0] = '\0';
	EndType = 0;
	kicktimeflag = false;
	hassendcard = false;
	m_bCurrRound = 0;
	m_bIsOverTimer = false;
	m_nMagicCoin = 10;
	m_nFirstIndex = 0;
	m_nDealerIndex = 0;
	m_vecBetCoin.clear();
	m_lSumPool = 0;
	m_lCurrBetMax = 0;
	this->setTableConf();
	m_GameLogic.CoppyCardList();
	m_lLeaveSumCoin = 0;
	m_bSetAllinCoinFlag = false;
	m_lHasAllInCoin = 0;
	m_nRaseUid = 0;
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
	CoinCfg* pCoincfg = CoinConf::getInstance()->getCoinCfg();
	
	this->m_nAnte = pCoincfg->ante > 0 ? pCoincfg->ante : 10;

	if (pCoincfg->tax > 100 || pCoincfg->tax <= 0)
		pCoincfg->tax = 5;
	this->m_nTax = pCoincfg->tax;
	m_nMaxLimit = pCoincfg->maxlimit;
	m_lMaxAllIn = pCoincfg->maxallin;
	m_nRase1    = pCoincfg->rase1;
	m_nRase2    = pCoincfg->rase2;
	m_nRase3	= pCoincfg->rase3;
	m_nRase4    = pCoincfg->rase4;
	m_nMagicCoin = pCoincfg->magiccoin;

	m_nMulCount = Configure::getInstance()->mulcount;
	m_nMulNum1 = Configure::getInstance()->mulnum1;
	m_nMulCoin1 = Configure::getInstance()->mulcoin1;
	m_nMulNum2 = Configure::getInstance()->mulnum2;
	m_nMulCoin2 = Configure::getInstance()->mulcoin2;

	m_nForbidCount = Configure::getInstance()->forbidcount;
	m_nForbidNum1 = Configure::getInstance()->forbidnum1;
	m_nForbidCoin1 = Configure::getInstance()->forbidcoin1;
	m_nForbidNum2 = Configure::getInstance()->forbidnum2;
	m_nForbidCoin2 = Configure::getInstance()->forbidcoin2;

	m_nChangeCount = Configure::getInstance()->changecount;
	m_nChangeCoin1 = Configure::getInstance()->changecoin1;
	m_nChangeCoin2 = Configure::getInstance()->changecoin2;
	m_nChangeCoin3 = Configure::getInstance()->changecoin3;
	_LOG_DEBUG_("m_nTax:%d m_nAnte:%d m_nMaxLimit:%d m_lMaxAllIn:%ld \n", m_nTax, m_nAnte, m_nMaxLimit, m_lMaxAllIn);
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

int Table::playerComming(Player* player)
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
				player->login();
				player->enter();
				player->m_nStatus = player_array[i]->m_nStatus;
				player->tid = player_array[i]->tid;
				player->m_nTabIndex = player_array[i]->m_nTabIndex;
				player->optype = player_array[i]->optype;
				player->m_bCardStatus = player_array[i]->m_bCardStatus;
				player->card_array[0] = player_array[i]->card_array[0];
				player->card_array[1] = player_array[i]->card_array[1];
				player->card_array[2] = player_array[i]->card_array[2];
				player_array[i]->init();
				player_array[i] = player;
			}
		}

        IProcess::NotifyPlayerNetStatus(this, player->id, 100, 1);

		return 0;
	}
	//说明此房间这个用户进入不成功，返回-2之后重新给此用户分配房间
	if(playerSeatInTab(player) < 0)
		return -2;

	player->login();
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
		return;
	}
	_LOG_WARN_("Tid[%d] Player[%d] Leave\n", this->id, player->id);
	player->leave();
	player->init();
	this->setSeatNULL(player);
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
	/*for(int i = 0; i < m_bMaxNum; ++i)
	{
		if(player_array[i] == NULL)
		{
			player_array[i] = complayer;
			complayer->m_nStatus = STATUS_PLAYER_COMING;
			complayer->tid = this->id;
			complayer->m_nTabIndex = i;
			return 0;
		}
	}*/

	//调整用户进入的顺序，促使进入如果只有两个用户则在对面
	if(player_array[0] == NULL)
	{
		player_array[0] = complayer;
		complayer->m_nStatus = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->m_nTabIndex = 0;
		return 0;
	}

	if(player_array[2] == NULL)
	{
		player_array[2] = complayer;
		complayer->m_nStatus = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->m_nTabIndex = 2;
		return 0;
	}
	//当前桌子只允许两个人
	if(m_bMaxNum == 2)
		return -1;

	if(player_array[1] == NULL)
	{
		player_array[1] = complayer;
		complayer->m_nStatus = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->m_nTabIndex = 1;
		return 0;
	}
	//当前桌子只允许三个人
	if(m_bMaxNum == 3)
		return -1;

	if(player_array[3] == NULL)
	{
		player_array[3] = complayer;
		complayer->m_nStatus = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->m_nTabIndex = 3;
		return 0;
	}

	//当前桌子只允许四个人
	if(m_bMaxNum == 4)
		return -1;

	if(player_array[4] == NULL)
	{
		player_array[4] = complayer;
		complayer->m_nStatus = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->m_nTabIndex = 4;
		return 0;
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
	this->m_bCurrRound = 1;
	this->setTableConf();
	m_GameLogic.RandCardList(m_bTableCardArray[0],sizeof(m_bTableCardArray)/sizeof(m_bTableCardArray[0][0]));

	//std::string debuginfo;
	int wCardCount = 0;
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		if (player)
		{
			//3天内新玩家充值超过100且输掉80%以上，该玩家30%机会赢1-3局
			//如果有赢的机会, 则要与同桌较大赢面的玩家形成30%可能的冤家牌
			/*
			if(3天内新玩家充值超过100且输掉80%以上)
			{
				if(rand()%10 < 3)
				{
					//预先比牌
					do{
						if(当前最大牌是同桌赢面较大的玩家BestP && rand()%10 < 3)
							从牌堆换出牌和BestP形成冤家牌
							break;
					
						//(当前最大牌不是同桌赢面较大的玩家BestP)将该玩家的牌和最大牌互换

					}while(0);
				}
			}

			*/
			memcpy(player->card_array,m_bTableCardArray[(wCardCount++)], sizeof(m_bTableCardArray[0]));
			m_GameLogic.SetAvailCardList(player->card_array, MAX_COUNT);
			player->m_bCardStatus = CARD_UNKNOWN;
			player->m_nStatus = STATUS_PLAYER_ACTIVE;
			BYTE TempCardDate[MAX_COUNT];
			memcpy(TempCardDate,player->card_array,sizeof(player->card_array));
			this->m_GameLogic.SortCardList(TempCardDate,MAX_COUNT);
			player->m_nEndCardType = this->m_GameLogic.GetCardType(TempCardDate, MAX_COUNT);
			char buffer[1024] = { 0 };
			sprintf(buffer, "uid[%d]:[%s]-[%s%s%s]\t", player->id,
				GameUtil::JinhuaCardType(player->m_nEndCardType).c_str(), GameUtil::PokerIntToString(player->card_array[0]).c_str(),
				GameUtil::PokerIntToString(player->card_array[1]).c_str(), GameUtil::PokerIntToString(player->card_array[2]).c_str());
			//debuginfo += std::string(buffer, strlen(buffer));

            //记录牌型，游戏结束时跟结算一起输出到日志
            player->clearRecordAnalysisInfo();
            std::string cards = std::string(buffer , strlen(buffer));
            player->recordCard(cards);

			AllocSvrConnect::getInstance()->userUpdateStatus(player, STATUS_PLAYER_ACTIVE);
			player->m_lMoney -= this->m_nAnte;
			player->m_lSumBetCoin += this->m_nAnte;
			m_lSumPool += this->m_nAnte;
			m_vecBetCoin.push_back(this->m_nAnte);
			player->m_nChangeNum = this->m_nChangeCount;
		}
	}
	//LOGGER(E_LOG_INFO) << ">>>>>tid=" << this->id << " " << debuginfo;

	m_lCurrBetMax = this->m_nAnte;
	m_BetMaxCardStatus = CARD_UNKNOWN;
	m_AllinCardStatus = CARD_UNKNOWN;
	NotifyUnknowCardMaxNum(NULL);

	//确定发牌位置
	if(this->m_nPreWinIndex >= 0)
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			short index = (m_nPreWinIndex + i)%m_bMaxNum;
			Player* player = player_array[index];
			if(player)
			{
				m_nDealerIndex = index;
				break;
			}
		}
	}
	else
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* player = player_array[i];
			if(player)
			{
				m_nDealerIndex = i;
				break;
			}
		}
	}

	//由发牌位置确定叫牌位置
	for(i = 1; i < m_bMaxNum; ++i)
	{
		short index = (m_nDealerIndex + i)%m_bMaxNum;
		Player* player = player_array[index];
		if(player)
		{
			m_nFirstIndex = index;
			break;
		}
	}

	//定义第一个叫牌玩家操作类型
	setPlayerOptype(this->player_array[m_nFirstIndex], 0);
	this->setPlayerlimitcoin();
	_LOG_DEBUG_("m_nDealerIndex:%d player:%d FirstIndex:%d\n", m_nDealerIndex, this->player_array[m_nFirstIndex]->id, m_nFirstIndex);

	this->m_nStatus = STATUS_TABLE_ACTIVE;
	this->setStartTime(time(NULL));
	time_t t;
	time(&t);
	char time_str[32]={0};
    struct tm* tp= localtime(&t);
    strftime(time_str,32,"%Y%m%d%H%M%S",tp);
    char gameId[64] ={0};
    short tlevel = this->m_nType;
    sprintf(gameId, "%s|%02d|%02d|%d", time_str,tlevel,this->m_nCountPlayer,this->player_array[m_nFirstIndex]->id);
    this->setGameID(gameId); 
	_LOG_INFO_("[gameStart] tid=[%d] gameid[%s]\n",this->id, this->getGameID());
	AllocSvrConnect::getInstance()->updateTableStatus(this);

	this->m_nCurrBetUid = this->player_array[m_nFirstIndex]->id;
	this->startBetCoinTimer(this->player_array[m_nFirstIndex]->id, Configure::getInstance()->betcointime);
	player_array[m_nFirstIndex]->setBetCoinTime(time(NULL));

}

int Table::PlayerChangeCard(Player* player, BYTE CardValue)
{
	if(player == NULL || CardValue <= 0)
		return -1;
	bool hasThisCard = false;
	for(int i = 0; i < MAX_COUNT; ++i)
	{
		if(player->card_array[i] == CardValue)
		{
			hasThisCard = true;
			break;
		}
	}

	if(!hasThisCard)
	{
		return -1;
	}

	BYTE RetCardValue = m_GameLogic.ChangeCard(player->card_array, MAX_COUNT, CardValue);
	if(RetCardValue <= 0)
	{
		return -1;
	}
	for(int i = 0; i < MAX_COUNT; ++i)
	{
		if(player->card_array[i] == CardValue)
		{
			player->card_array[i] = RetCardValue;
			break;
		}
	}
	char cardbuff[64]={0};
	for(int j = 0; j < MAX_COUNT; ++j)
	{
		sprintf(cardbuff+5*j, "0x%02x ",player->card_array[j]);
	}
	m_GameLogic.SetAvailCardList(player->card_array, MAX_COUNT);
	BYTE TempCardDate[MAX_COUNT];
	memcpy(TempCardDate,player->card_array,sizeof(player->card_array));
	this->m_GameLogic.SortCardList(TempCardDate,MAX_COUNT);
	player->m_nEndCardType = this->m_GameLogic.GetCardType(TempCardDate, MAX_COUNT);
	_LOG_INFO_("[PlayerChangeCard] tid=[%d] uid=[%d] cardarray:[%s] cardtype:[%d]\n", this->id, player->id, cardbuff, player->m_nEndCardType);
	player->m_nChangeNum--;
	return 0;
}

void Table::playerBetCoin(Player* player, int64_t betmoney)
{
	if(player == NULL)
		return;
	player->m_lSumBetCoin += betmoney;
	player->m_lMoney -= betmoney;
	this->m_lSumPool += betmoney;
	m_vecBetCoin.push_back(betmoney);
}

int Table::compareCard(Player* player, Player* opponent)
{
	if(player == NULL || opponent == NULL)
		return -1;
	player->m_setCompare.insert(opponent->id);
	opponent->m_setCompare.insert(player->id);
	return this->m_GameLogic.CompareCard(player->card_array, opponent->card_array, MAX_COUNT);
}

void Table::setPlayerOptype(Player* opplayer, short playeroptype)
{
	static  CoinCfg *concfg = CoinConf::getInstance()->getCoinCfg();
	if(opplayer == NULL)
		return ;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player *player = this->player_array[i];
		if(player && player->isActive())
		{
			player->optype = 0;
			
			if(this->iscanGameOver())
			{
				player->optype = OP_CALLING;
				continue;
			}
			//玩家未输牌时定义玩家操作权限
			if(player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/)
			{
				if (this->m_bCurrRound < concfg->compare_round)
				{
					player->optype = OP_CALL|OP_RASE|OP_THROW;
				}
				else
				{
					player->optype = OP_CALL|OP_RASE|OP_THROW|OP_COMPARE;
				}

				if(opplayer->id != player->id)
				{
					player->optype = OP_THROW;
				}
				else
				{
					//上一个人是Allin轮到自己肯定只有Allin和弃牌选择，或者当前已经到了可以allin的轮数了
					if(this->m_bCurrRound >= Configure::getInstance()->allin_round)
					{
						if(Configure::getInstance()->betmaxallinflag == 1)
						{
							if(this->m_lCurrBetMax >= this->m_nMaxLimit)
							{
								player->optype = OP_CALL|OP_ALLIN|OP_THROW|OP_COMPARE;
								m_bSetAllinCoinFlag = true;
							}
						}
						else
						{
							player->optype = OP_CALL|OP_ALLIN|OP_THROW|OP_COMPARE;
							m_bSetAllinCoinFlag = true;
						}

					}
					if(this->hasSomeOneAllin())
					{
						player->optype = OP_ALLIN|OP_THROW|OP_CALLING;
						m_bSetAllinCoinFlag = true;
					}
				}

				//如果当前是最后一轮，然后当第一个说话的人没有全下则其他后面的人也不能全下
				if(this->m_bCurrRound == Configure::getInstance()->max_round)
				{
					if(player_array[this->m_nFirstIndex])
					{
						//表示第一个用户已经说话了
						if(player_array[this->m_nFirstIndex]->id != opplayer->id)
						{
							//最后一轮第一个人没有allin 并且前面也没有allin直接把allin剔除
							if(!player_array[this->m_nFirstIndex]->m_bAllin)
							{
								m_bSetAllinCoinFlag = false;
								player->optype &= 0x07B; //剔除掉全下的类型 
								if(this->hasSomeOneAllin())
								{
									m_bSetAllinCoinFlag = true;
									player->optype |= OP_ALLIN;
								}
							}
							//剔除掉加注类型，第一个人没加注则后面的人也不能加注
							//player->optype &= 0x07D; //剔除掉加注的类型 
							//ULOGGER(E_LOG_INFO, player->id) << "0x07d----1";
						}
					}
				}

				if(playeroptype == OP_ALLIN)
					player->optype |= OP_CALLING; 
				if (player->m_bCardStatus == CARD_UNKNOWN && this->m_BetMaxCardStatus == CARD_UNKNOWN) 
				{
					if (this->m_lCurrBetMax >= this->m_nMaxLimit)
					{
						ULOGGER(E_LOG_INFO, player->id) << "0x07d----2 card status = " << player->m_bCardStatus << " max card status = " << m_BetMaxCardStatus;
						player->optype &= 0x07D; //剔除掉加注的类型
					}
				}
				else
				{
					if (this->m_lCurrBetMax >= this->m_nMaxLimit * 2)
					{
						ULOGGER(E_LOG_INFO, player->id) << "0x07d----3 card status = " << player->m_bCardStatus;
						player->optype &= 0x07D; //剔除掉加注的类型
					}
				}
				if (player->m_bCardStatus == CARD_UNKNOWN && this->m_bCurrRound > concfg->check_round)
					player->optype |= OP_CHECK;

				if(player->m_bAllin)
					player->optype = OP_THROW | OP_CALLING;
				
				//当前要没有人Allin
				if(!this->hasSomeOneAllin())
				{
					//当前金币不够跟注，则把用户的全下去掉加注去掉
					if((opplayer->m_bCardStatus == CARD_UNKNOWN && opplayer->m_lMoney < this->m_lCurrBetMax) ||
						(opplayer->m_bCardStatus == CARD_KNOWN && opplayer->m_lMoney < this->m_lCurrBetMax * 2))
					{
						if(player->id == opplayer->id)
						{
							//去除掉加注，全下按钮，跟到底按钮
							player->optype &= 0x079;
							player->optype |= OP_CALLING;
							player->optype |= OP_COMPARE;
						}
					}
				}
				
			}
		}
	}
	return;
}

void Table::setNextFirstPlayer(Player* player)
{
	if(player == NULL)
		return;
	for(int i = 1; i < m_bMaxNum; ++i)
	{
		short index = (player->m_nTabIndex + i)%m_bMaxNum;
		if(this->player_array[index] && this->player_array[index]->isActive() && this->player_array[index]->m_bCardStatus > CARD_DISCARD /*&& this->player_array[index]->m_bCardStatus != CARD_LOSE*/)
		{
			this->m_nFirstIndex = this->player_array[index]->m_nTabIndex;
			break;
		}
	}
}

void Table::setPlayerlimitcoin()
{
	if(!m_bSetAllinCoinFlag)
		return;
	m_lCanMaxBetCoin = m_lMaxAllIn;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player *player = this->player_array[i];
		if(player && player->isActive() && player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/)
		{
			if(player->m_lMoney < m_lCanMaxBetCoin)
				m_lCanMaxBetCoin = player->m_lMoney;
		}
	}

	if(this->m_lHasAllInCoin > 0)
		m_lCanMaxBetCoin = this->m_lHasAllInCoin;

	_LOG_DEBUG_("=-----------------m_lCanMaxBetCoin:%ld m_lMaxAllIn:%ld \n", m_lCanMaxBetCoin, m_lMaxAllIn);
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player *player = this->player_array[i];
		if(player && player->isActive() && player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/)
			player->m_AllinCoin  = m_lCanMaxBetCoin;
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
		if(player && player->isActive() && player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/)
		{
			return false;
		}
	}
	return true;
}

Player* Table::getOtherPlayer(Player* curPlayer)
{
    if (curPlayer == NULL)
    {
        _LOG_ERROR_( "the assigned player is NULL");
        return NULL;
    }
    
    for (int i = 1; i < m_bMaxNum; ++i)
    {
        int index = (curPlayer->m_nTabIndex + i) % m_bMaxNum;
        Player* otherplayer = this->player_array[index];
        if (otherplayer && otherplayer->id != curPlayer->id && otherplayer->isActive() && otherplayer->m_bCardStatus > 0)
        {
            return otherplayer;
        }
    }

    return NULL;
}

int Table::getActivePlayingPlayerCount()
{
    int count = 0;
    for (int i = 0; i < m_bMaxNum; ++i)
    {
        Player* p = this->player_array[i];
        if (p && p->isActive() && p->m_bCardStatus > 0)
        {
            ++count;
        }
    }

    return count;
}

Player* Table::getNextBetPlayer(Player* player, short playeroptype)
{
	if(player == NULL)
		return NULL;
	this->stopBetCoinTimer();
	//说明此用户已经有操作行为
	if(this->m_bCurrRound != 1)
		player->timeoutCount = 0;
	for(int i = 1; i < m_bMaxNum; ++i)
	{
		int player_index = (player->m_nTabIndex+i)%m_bMaxNum;
		Player* otherplayer = this->player_array[player_index];
		if(otherplayer && otherplayer->id != player->id && otherplayer->isActive() && 
			otherplayer->m_bCardStatus > 0 && !otherplayer->m_bAllin)
		{
			if(otherplayer->m_nTabIndex == this->m_nFirstIndex)
			{
				this->m_bCurrRound++;
				if(this->hasSomeOneForbid())
				{
					for(int j = 0; j < m_bMaxNum; ++j)
					{
						Player* forbider = this->player_array[j];
						if(forbider && forbider->m_bCardStatus > CARD_DISCARD /*&& forbider->m_bCardStatus != CARD_LOSE*/ && forbider->m_nUseForbidNum > 0)
							forbider->m_nUseForbidNum--;
					}
					IProcess::pushForbidinfo(this);
				}
			}
			if(this->m_bCurrRound > Configure::getInstance()->max_round)
			{
				_LOG_INFO_("getNextBetPlayer : max round and game over.\n");
				gameOver();
				this->m_bCurrRound = Configure::getInstance()->max_round;
				return NULL;
			}
			this->m_nCurrBetUid = otherplayer->id;
			this->setPlayerOptype(otherplayer, playeroptype);
			this->startBetCoinTimer(otherplayer->id,Configure::getInstance()->betcointime);
			otherplayer->setBetCoinTime(time(NULL));		
			this->setPlayerlimitcoin();
			return otherplayer;
		}
	}
	_LOG_INFO_("getNextBetPlayer : not found.\n");
	this->setPlayerOptype(player, 0);
	return NULL;
}

int Table::calcCoin()
{
	int i = 0;
	short hasCardNum = 0;
	Player* winner = NULL;	// 如果只有一个玩家的话，那Ta就是Winner
	for(i = 0; i < m_bMaxNum; ++i)
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->isGameOver() && getplayer->m_bCardStatus > CARD_DISCARD /*&& getplayer->m_bCardStatus != CARD_LOSE*/)
		{
			winner = getplayer;
			hasCardNum++;
		}
		//处理用户下注的数额加到用户自己身上，后面便于和最终输赢额度匹配
		if(getplayer && getplayer->m_lSumBetCoin > 0)
		{
			getplayer->m_lMoney += getplayer->m_lSumBetCoin;
		}
	}

	LOGGER(E_LOG_INFO) << "hasCardNum = " << hasCardNum;

	/////////////////////////////////////////////////////////////////////////
	// 只有一个未弃牌玩家的情况
	/////////////////////////////////////////////////////////////////////////

	if(hasCardNum == 1)
	{
		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* getplayer = this->player_array[i];
			if(getplayer && winner->id != getplayer->id && getplayer->isGameOver() && getplayer->m_lSumBetCoin != 0)
			{
				set_loser_coin(this, winner, getplayer);
			}
		}

		winner->setFinalGetCoin(m_lSumPool - winner->m_lSumBetCoin);

		return 0;
	}

	/////////////////////////////////////////////////////////////////////////
	// 有多于一个未弃牌玩家的情况
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// 到这步了结束的话所有下过注的玩家都可以看到任何一方牌

	for (i=0;i<m_bMaxNum;i++)
	{
		Player* getplayer = this->player_array[i];
		if(getplayer && getplayer->m_bCardStatus > CARD_DISCARD /*&& getplayer->m_bCardStatus != CARD_LOSE*/)
		{
			for(int j=0;j<m_bMaxNum;j++)
			{
				Player* player = this->player_array[j];
				if(player && player->id != getplayer->id && player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/)
				{
					getplayer->m_setCompare.insert(player->id);
				}
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// 确定赢家

	// 查找最大玩家
	for (i=0;i<m_bMaxNum;i++)
	{
		Player* getplayer = this->player_array[i];
		if (getplayer == NULL ||
			getplayer->id == winner->id ||
			(getplayer->m_bCardStatus == CARD_DISCARD /*|| getplayer->m_bCardStatus == CARD_LOSE*/))
		{
			continue;
		}

		//对比扑克
		if (m_GameLogic.CompareCard(getplayer->card_array,winner->card_array,MAX_COUNT)==1) 
		{
			winner = getplayer;
		}
	}

	// 查找所有牌最大的玩家
	std::vector<Player *> tempDrawer;
	tempDrawer.reserve(m_bMaxNum);
	tempDrawer.push_back(winner);

	for (i=0;i<m_bMaxNum;i++)
	{
		Player* getplayer = this->player_array[i];
		if (getplayer == NULL ||
			getplayer->id == winner->id ||
			(getplayer->m_bCardStatus == CARD_DISCARD /*|| getplayer->m_bCardStatus == CARD_LOSE*/))
		{
			continue;
		}

		if (m_GameLogic.CompareCard(getplayer->card_array,winner->card_array,MAX_COUNT)==DRAW) 
		{
			tempDrawer.push_back(getplayer);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// 根据赢家数量的不同来进行结算

	size_t tempdrawnum = tempDrawer.size();
	LOGGER(E_LOG_INFO) << "tempdrawnum = " << tempdrawnum;

	if (tempdrawnum < 2)
	{
		// 多个未弃牌的玩家中只有一个赢家的情况

		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* getplayer = this->player_array[i];
			if(getplayer && winner->id != getplayer->id && getplayer->isGameOver() && getplayer->m_lSumBetCoin != 0)
			{
				set_loser_coin(this, winner, getplayer, true);
			}
		}
		winner->setFinalGetCoin(this->m_lSumPool - winner->m_lSumBetCoin);
	}
	else
	{
		// 多个未弃牌的玩家中有多个赢家的情况

		for(i = 0; i < m_bMaxNum; ++i)
		{
			Player* getplayer = this->player_array[i];
			if(getplayer && getplayer->isGameOver() && getplayer->m_lSumBetCoin != 0)
			{
				bool bcal = true;	// 是否要参与结算
				for(size_t j = 0; j < tempdrawnum; ++j)
				{
					if(tempDrawer[j]->id == getplayer->id)
					{
						bcal = false;
						break;
					}
				}

				if(bcal)
				{
					set_loser_coin(this, winner, getplayer);
				}
				else
				{
					m_lSumPool -= getplayer->m_lSumBetCoin;	// 上下计算方式统一，统一通过总奖池来处理
				}
			}
		}

		int64_t average_win_coin = m_lSumPool / tempdrawnum;
		for(size_t j = 0; j < tempdrawnum; ++j)
		{
			tempDrawer[j]->setFinalGetCoin(average_win_coin);
		}
	}

	return 0;
}

bool Table::hasSomeOneAllin()
{
	if(!this->isActive())
		return false;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		//此人必须是active状态，并且有手牌
		if(player && player->isActive() && player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/ && player->m_bAllin)
		{
			return true;
		}
	}
	return false;
}

bool Table::hasSomeOneForbid()
{
	if(!this->isActive())
		return false;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		//此人必须是active状态，并且有手牌
		if(player && player->isActive() && player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/ && player->m_nUseForbidNum > 0)
		{
			return true;
		}
	}
	return false;
}

bool Table::iscanGameOver()
{
	if(!this->isActive())
		return false;
	short activeCount = 0;
	short allinCount = 0;
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = player_array[i];
		//此人必须是active状态，并且有手牌
		if(player && player->isActive() && player->m_bCardStatus > CARD_DISCARD /*&& player->m_bCardStatus != CARD_LOSE*/)
		{
			if(player->m_bAllin)
				++allinCount;
			++activeCount;
		}
	}
	//printf("++++++++++allinCount:%d+++++++++++activeCount:%d\n",allinCount, activeCount);
	if(activeCount == allinCount)
		return true;
	if(activeCount > 1)
		return false;
	return true;
}

int Table::gameOver(bool compareflag)
{
	this->stopBetCoinTimer();
	//设置上一盘赢牌的用户
	int i = 0;

	_LOG_INFO_("[gameOver] gameid[%s]\n", this->getGameID());
	if(compareflag)
		this->startGameOverTimer(COMPARE_OVER_TIME);
	else
		this->startGameOverTimer(GAME_OVER_TIME);
	return 0;
}

void Table::setKickTimer()
{
	if (this->isActive() || this->m_nCountPlayer <= 1)
		return;
	this->startKickTimer(Configure::getInstance()->kicktime);
	this->lockTable();
	for(int i = 0; i < m_bMaxNum; ++i)
	{
		Player* player = this->player_array[i];
		if(player && (!player->isActive() || !player->isReady())&& player->startnum == 1)
		{
			IProcess::serverWarnPlayerKick(this, player, Configure::getInstance()->kicktime - 2);
		}
	}
}

bool Table::CheckIP(std::string strip)
{
	if (Configure::getInstance()->m_nAllowSameIp == 1)
	{
		return true;
	}
	if (strip.size() < 4)
		return true;

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (this->player_array[i] && this->player_array[i]->source != 30)
		{
			if (strlen(this->player_array[i]->m_byIP) < 2)
				continue;

			if(strcmp(this->player_array[i]->m_byIP, strip.c_str()) == 0)
			{
				return false;
			}
		}
	}
	return true;
}

bool Table::setCompareResultInfo(int result  , Player* p1 , Player* p2, int64_t cmpMoney)
{
    if (!p1 || !p2 || cmpMoney < 0)
    {
        _LOG_ERROR_("table %d, the players of caompare card not right !!" , id);
        return false;
    }

    _LOG_DEBUG_("table %d, %d and %d caompare card , not notify, save and delay notify after 3 seconds !!" , id , p1->id , p2->id);

    CmpRInfo.p1 = p1;
    CmpRInfo.p2 = p2;
    CmpRInfo.result = result;
    CmpRInfo.cmpMoney = cmpMoney;
    return true;
}

void Table::ClearCompareResultInfo()
{
    CmpRInfo.p1 = NULL;
    CmpRInfo.p2 = NULL;
    CmpRInfo.result = -1;
    CmpRInfo.cmpMoney = -1;
}


bool Table::isLoseWithin3Day(Player* pl)
{
	Player *tp = pl;
	_LOG_DEBUG_("current player uid: %d, total money: %lld\n", tp->id, tp->m_lTotalMoney);
	//获取注册时间
	tp->m_Redis.HGET();

	//获取充值记录

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

void Table::startCompareResultNotifyTimer(int timeout)
{
    _LOG_DEBUG_("=========[table id %d ]\n", id);
    timer.startCompareResultNotifyTimer(timeout);
}

void Table::stopCompareResultNotifyTimer()
{
    _LOG_DEBUG_("=========[table id %d]\n", id);
    timer.stopCompareResultNotifyTimer();
}

void Table::NotifyUnknowCardMaxNum(Player* player)
{
	if (NULL == player)
	{
		for (BYTE i = 0; i < m_bMaxNum; ++i)
		{
			if (player_array[i])
			{
				OutputPacket response;
				uint64_t curMaxNum = FormatValue(m_BetMaxCardStatus, m_lCurrBetMax, player_array[i]->m_bCardStatus);

				response.Begin(GMSERVER_MAX_NUM, player_array[i]->id);
				response.WriteShort(0);
				response.WriteString("");
				response.WriteInt(player_array[i]->id);
				response.WriteInt64(curMaxNum);
				response.End();
				HallManager::getInstance()->sendToHall(player_array[i]->m_nHallid, &response, false);
			}
		}
	}
	else
	{
		OutputPacket response;
		uint64_t curMaxNum = FormatValue(m_BetMaxCardStatus, m_lCurrBetMax, player->m_bCardStatus);

		response.Begin(GMSERVER_MAX_NUM, player->id);
		response.WriteShort(0);
		response.WriteString("");
		response.WriteInt(player->id);
		response.WriteInt64(m_lCurrBetMax);
		response.End();
		HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
	}
}

int64_t Table::FormatValue(BYTE fromType, int64_t fromValue, BYTE toType)
{
	static auto coincfg = CoinConf::getInstance()->getCoinCfg();
	if (fromType == toType || fromValue == coincfg->rase4)
		return fromValue;

	int64_t toValue;
	if (CARD_UNKNOWN == fromType)
	{
		if (coincfg->rase2 == fromValue)
			toValue = coincfg->rase3;
		else
			toValue = (fromValue << 1);
	}
	else
	{
		if (coincfg->rase3 == fromValue)
			toValue = coincfg->rase2;
		else
			toValue = (fromValue >> 1);
	}

	return toValue;
}

