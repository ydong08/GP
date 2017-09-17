#include <stdio.h>
#include <stdlib.h>
#include "Table.h"
#include "AllocSvrConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "IProcess.h"
#include "TaskRedis.h"
#include "CoinConf.h"
#include "Util.h"
#include "OperationRedis.h"

Table::Table():status(-1)
{	 
}
Table::~Table()
{
}

void Table::init()
{
	timer.init(this);
	stopAllTimer();
	this->status = STATUS_TABLE_EMPTY;
	clevel = Configure::getInstance()->level;
	ante = 0;
	tax = 0;
	memset(player_array, 0, sizeof(player_array));
	memset(m_bTableCardArray, 0, sizeof(m_bTableCardArray));
	countPlayer = 0;
	currRound = 0;
	currMaxCoin = 0;
	raseUid = 0;
	betUid = 0;
	commonCoin = 0;
	raterent = 0;
	maxbetcoin = 0;
	pregame_winner = NULL;
	memset(leaverarry, 0, sizeof(leaverarry));
	leavercount=0;
	memset(winnerorder, 0, sizeof(winnerorder));
	winnercount = 0;
	maxCount = Configure::getInstance()->numplayer;
	thisGameLimit = 0;

	owner = 0;
	haspasswd = false;
	tableName[0] = '\0';
	password[0] = '\0';
	GameID[0] = '\0';
	EndType = 0;
	kicktimeflag = false;
	isthrowwin = false;
	m_nRePlayTime = 0;
	SendCardTime = 0;
	hassendcard = false;

	m_nRoundNum1 = 0;
	m_nRoundNum2 = 0;
	m_nRoundNum3 = 0;
	m_nGetCoinHigh1 = 0;
	m_nGetCoinLow1 = 0;
	m_nGetCoinHigh2 = 0;
	m_nGetCoinLow2 = 0;
	m_nGetCoinHigh3 = 0;
	m_nGetCoinLow3 = 0;
	m_bRewardType = 1;
	m_nGetRollHigh1 = 0;
	m_nGetRollLow1 = 0;
	m_nGetRollHigh2 = 0;
	m_nGetRollLow2 = 0;
	m_nGetRollHigh3 = 0;
	m_nGetRollLow3 = 0;
	m_nMagicCoin = 1;
}

void Table::reset()
{
	currRound = 0;
	currMaxCoin = 0;
	raseUid = 0;
	betUid = 0;
	commonCoin = 0;
	raterent = 0;
	maxbetcoin = 0;
	memset(leaverarry, 0, sizeof(leaverarry));
	leavercount=0;
	memset(winnerorder, 0, sizeof(winnerorder));
	winnercount = 0;
	GameID[0] = '\0';
	EndType = 0;
	thisGameLimit = 0;
	memset(m_bTableCardArray, 0, sizeof(m_bTableCardArray));
	kicktimeflag = false;
	isthrowwin = false;
	SendCardTime = 0;
	hassendcard = false;
	m_nRoundNum1 = 0;
	m_nRoundNum2 = 0;
	m_nRoundNum3 = 0;
	m_nGetCoinHigh1 = 0;
	m_nGetCoinLow1 = 0;
	m_nGetCoinHigh2 = 0;
	m_nGetCoinLow2 = 0;
	m_nGetCoinHigh3 = 0;
	m_nGetCoinLow3 = 0;
	m_bRewardType = 1;
	m_nGetRollHigh1 = 0;
	m_nGetRollLow1 = 0;
	m_nGetRollHigh2 = 0;
	m_nGetRollLow2 = 0;
	m_nGetRollHigh3 = 0;
	m_nGetRollLow3 = 0;
	this->setRoundTaskConf();
}

Player* Table::getPlayer(int uid)
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player && player->id == uid)
			return player;
	}
	return NULL;
}

bool Table::isUserInTab(int uid)
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player && player->id == uid)
			return true;
	}
	return false;
}

int Table::playerSeatInTab(Player* complayer)
{
	/*for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player_array[i] == NULL)
		{
			player_array[i] = complayer;
			complayer->status = STATUS_PLAYER_COMING;
			complayer->tid = this->id;
			complayer->tab_index = i;
			return 0;
		}
	}*/
	//调整用户进入的顺序，促使进入如果只有两个用户则在对面
	if(player_array[0] == NULL)
	{
		player_array[0] = complayer;
		complayer->status = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->tab_index = 0;
		return 0;
	}

	if(player_array[2] == NULL)
	{
		player_array[2] = complayer;
		complayer->status = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->tab_index = 2;
		return 0;
	}
	//当前桌子只允许两个人
	if(maxCount == 2)
		return -1;

	if(player_array[1] == NULL)
	{
		player_array[1] = complayer;
		complayer->status = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->tab_index = 1;
		return 0;
	}
	//当前桌子只允许三个人
	if(maxCount == 3)
		return -1;

	if(player_array[3] == NULL)
	{
		player_array[3] = complayer;
		complayer->status = STATUS_PLAYER_COMING;
		complayer->tid = this->id;
		complayer->tab_index = 3;
		return 0;
	}

	return -1;
}

int Table::playerComming(Player* player)
{
	if(player == NULL)
		return -1;
	//如果此用户已经在此桌子则把另外那个player清除，替代为新的player
	if(isUserInTab(player->id))
	{
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player_array[i] && player_array[i]->id == player->id)
			{
				player->status = player_array[i]->status;
				player->tid = player_array[i]->tid;
				player->tab_index = player_array[i]->tab_index;
				player->thisroundhasbet = player_array[i]->thisroundhasbet;
				player->hasallin = player_array[i]->hasallin;
				player->optype = player_array[i]->optype;
				player->hascard = player_array[i]->hascard;
				player->nextlimitcoin = player_array[i]->nextlimitcoin;
				player->carrycoin = player_array[i]->carrycoin;
				player->betCoinList[0] = player_array[i]->betCoinList[0];
				player->betCoinList[1] = player_array[i]->betCoinList[1];
				player->betCoinList[2] = player_array[i]->betCoinList[2];
				player->betCoinList[3] = player_array[i]->betCoinList[3];
				player->betCoinList[4] = player_array[i]->betCoinList[4];
				player->card_array[0] = player_array[i]->card_array[0];
				player->card_array[1] = player_array[i]->card_array[1];
				player->card_array[2] = player_array[i]->card_array[2];
				player->card_array[3] = player_array[i]->card_array[3];
				player->card_array[4] = player_array[i]->card_array[4];
				player_array[i]->init();
				player_array[i] = player;
			}
		}
		return 0;
	}
	//说明此房间这个用户进入不成功，返回-2之后重新给此用户分配房间
	if(playerSeatInTab(player) < 0)
		return -2;
	//如果桌子不是在玩牌则状态变为等待准备状态
	if(!this->isActive())
	{
		this->status = STATUS_TABLE_READY;
	}
	//这里设置用户进入的标志，并且设置状态
	player->enter();
	//当前用户数加1
	++this->countPlayer;

	if(this->countPlayer == 1)
		this->setRoundTaskConf();
	AllocSvrConnect::getInstance()->updateTableUserCount(this);
	return 0;
}

void Table::setSeatNULL(Player* player)
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player_array[i] == player)
			player_array[i] = NULL;
	}
}

void Table::playerLeave(int uid)
{
	Player*  player = this->getPlayer(uid);
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
	//清空上一个赢牌的用户
	if(pregame_winner)
	{
		if(pregame_winner->id == player->id)
			pregame_winner = NULL;
	}
	player->leave();
	player->init();
	this->setSeatNULL(player);
	//如果桌子不是在玩牌则状态变为等待准备状态
	if(!this->isActive())
	{
		this->status = STATUS_TABLE_READY;
	}
	//当前用户减一
	--this->countPlayer;
	//房间里面只剩一个人的时候直接解锁
	if(this->countPlayer == 1)
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


bool Table::isAllReady()
{
	int readyCount=0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player_array[i] && player_array[i]->isReady())
			readyCount++;
		if(readyCount == this->countPlayer)
			return true;
	}
	return false;
}

bool Table::isAllRobot()
{
	for(int i = 0; i < this->leavercount; ++i)
	{
		if(leaverarry[i].source != 30)
			return false;
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
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

bool Table::isCanGameStart()
{
	int readyCount=0;
	for(int i = 0; i < GAME_PLAYER; ++i)
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
	m_GameLogic.RandCard(m_bTableCardArray[0],sizeof(m_bTableCardArray)/sizeof(m_bTableCardArray[0][0]));
	//这里是手动做牌，给牌型任务做测试
	/*BYTE cardArray1[5] = {0x29, 0x11, 0x1b, 0x38, 0x09 };
	memcpy(m_bTableCardArray[0],cardArray1, sizeof(m_bTableCardArray[0]));
	//m_bTableCardArray[1] = {0x18,0x1c,0x1d,0x3a,0x3d};
	BYTE cardArray2[5] = {0x3b, 0x1a, 0x39, 0x1c, 0x2d};
	memcpy(m_bTableCardArray[2],cardArray2, sizeof(m_bTableCardArray[2]));
	//m_bTableCardArray[3] = {0x08,0x09,0x1a,0x2b,0x3c};*/


	int randnum = rand()%100;
	//randnum = 0;
	//高级场
	if((Configure::getInstance()->level == 3 || Configure::getInstance()->level == 4)&& randnum < Configure::getInstance()->loserate)
	{
		for(i = 0; i < GAME_PLAYER; ++i)
		{
			char cardbuff[64]={0};
			for(j = 0; j < 5; ++j)
			{
				sprintf(cardbuff+5*j, "0x%02x ",m_bTableCardArray[i][j]);
			}
			_LOG_DEBUG_("[gameStart] tid=[%d] cardarray:[%s]\n", this->id,cardbuff);
		}

		short templowindex = -1;
		for(i = 0; i < GAME_PLAYER; ++i)
		{
			if(player_array[i])
			{
				player_array[i]->isbacklist = CoinConf::getInstance()->isUserInBlackList(player_array[i]->id);
				if(player_array[i]->isbacklist)
				{
					templowindex = i;
					break;
				}
			}
		}
		printf("templowindex:%d \n", templowindex);
		if(templowindex >= 0)
		{
			BYTE templowcard[5] = {0};
			short tempindex = 0;
			for(i = 0; i < GAME_PLAYER; ++i)
			{
				if(player_array[i])
				{
					memcpy(templowcard, m_bTableCardArray[i], 5);
					tempindex = i;
					break;
				}
			}
			for(j = 0; j < GAME_PLAYER; ++j)
			{
				if(i != j && player_array[j])
				{
					BYTE tempfirst[5];
					BYTE tempsecond[5];
					memcpy(tempfirst, templowcard, 5);
					memcpy(tempsecond, m_bTableCardArray[j], 5);
					bool retflag = m_GameLogic.CompareCard(tempfirst, tempsecond, 5);
					printf("j:%d retflag:%s\n", j, retflag ? "true":"flase");
					if(retflag)
					{
						memcpy(templowcard, m_bTableCardArray[j], 5);
						tempindex = j;
					}
				}
			}
			printf("templowindex:%d tempindex:%d \n", templowindex, tempindex);

			if(templowindex != tempindex)
			{
				memcpy(m_bTableCardArray[tempindex], m_bTableCardArray[templowindex], 5);
				memcpy(m_bTableCardArray[templowindex], templowcard, 5);
			}
		}

	}

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		char cardbuff[64]={0};
		for(j = 0; j < 5; ++j)
		{
			sprintf(cardbuff+5*j, "0x%02x ",m_bTableCardArray[i][j]);
		}
		_LOG_DEBUG_("[gameStart] tid=[%d] cardarray:[%s]\n", this->id,cardbuff);
	}

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player)
		{
			memcpy(player->card_array,m_bTableCardArray[i], sizeof(m_bTableCardArray[0]));
			char cardbuff[64]={0};
			for(j = 0; j < 5; ++j)
			{
				sprintf(cardbuff+5*j, "0x%02x ",player->card_array[j]);
			}
			BYTE bCardArray[5];
			memcpy(bCardArray, player->card_array, 5);
			m_GameLogic.SortCard(bCardArray, 5);
			_LOG_INFO_("[gameStart] tid=[%d] uid=[%d] cardarray:[%s] cardType:%d\n", this->id, player->id, cardbuff, m_GameLogic.GetCardKind(bCardArray, 5));
			player->hascard = true;
			player->status = STATUS_PLAYER_ACTIVE;
			if(player->istask)
			{
				if(Configure::getInstance()->level <= 3)
				{
					int randnum = rand()%100;
					int  CurTotal = TaskRedis::getInstance()->getPlayerCompleteCount(player->id);
					if( CurTotal < Configure::getInstance()->curBeforeCount )
					{
						int randTemp = Configure::getInstance()->esayTaskProbability;
						if( randTemp != 100 )
						{
							randTemp += rand()%(100 - randTemp);
						}

						if( randnum < randTemp )
						{
							player->task1 = TaskManager::getInstance()->getRandEsayTask();
						}
					}
						
					/*if( (player->task1 == NULL) && （randnum < Configure::getInstance()->esayRandNum） )
					{
						player->task1 = TaskManager::getInstance()->getRandEsayTask();
					}*/

					if(player->task1 == NULL)
					{
						player->task1 = TaskManager::getInstance()->getRandTask();
					}

					if(player->task1)
					{
						player->ningot = player->task1->ningotlow + rand()%(player->task1->ningothigh - player->task1->ningotlow + 1);
						_LOG_INFO_("player[%d] get taskid[%ld] taskname[%s] level[%d]\n", player->id, player->task1->taskid, player->source == 1 ? player->task1->taskANDname : player->task1->taskIOSname, this->clevel);
					}
				}
			}
			AllocSvrConnect::getInstance()->userUpdateStatus(player, STATUS_PLAYER_ACTIVE);
		}
	}
	//设置桌子的台费规则
	this->setTableRent();
	//设置用户携带的金币数量
	//this->setPlayerCarryCoin();
	this->status = STATUS_TABLE_ACTIVE;
	this->setStartTime(time(NULL));
	time_t t;
	time(&t);
	char time_str[32]={0};
    struct tm* tp= localtime(&t);
    strftime(time_str,32,"%Y%m%d%H%M%S",tp);
    char gameId[64] ={0};
	short tlevel = this->clevel;
    sprintf(gameId, "%s|%02d|%02d|%d|%d|%d|%d", time_str,tlevel,this->countPlayer,
		player_array[0]?player_array[0]->id:0,
		player_array[1]?player_array[1]->id:0,
		player_array[2]?player_array[2]->id:0,
		player_array[3]?player_array[3]->id:0);
    this->setGameID(gameId);
	_LOG_INFO_("[gameStart] tid=[%d] raterent[%d] ante[%d] gameid[%s]\n",this->id, this->raterent, this->ante, this->getGameID());
	AllocSvrConnect::getInstance()->updateTableStatus(this);

	
	//把当前这局的金币数限制为带入金币第二大的人
	this->thisGameLimit = getSecondLargeCoin();
}


void Table::setTableRent()
{
	//这主要是解决游戏还没有开始桌费配置修改的情况
	//找到当前低注类型，如果这个桌子这几人中有读到配置是固定台费和按比率台费的则默认按固定台费收
	//如果都是一种类型，则按照里面最小的那个人确定这个桌子的台费
	// 	int64_t minmoney = 0x7FFFFFFFFFFFFF;
	// 	short minrate = 1000;
	// 	for(int i = 0; i < GAME_PLAYER; ++i)
	// 	{
	// 		Player* player = player_array[i];
	// 		if(player && player->isActive())
	// 		{
	// 			if(player->coincfg.raterent < minrate)
	// 			{
	// 				minrate = player->coincfg.raterent;
	// 			}
	// 			if(player->carrycoin < minmoney)
	// 				minmoney = player->carrycoin;
	// 		}
	// 	}
	// 	this->raterent = minrate;
	// 	if(minmoney/Configure::getInstance()->fraction < 1)
	// 		this->ante = 1;
	// 	else
	// 		this->ante = (int64_t)minmoney/Configure::getInstance()->fraction;

	CoinCfg cfg;	
	cfg.level = Configure::getInstance()->level;
	CoinConf::getInstance()->getCoinCfg(&cfg);
	this->tax  = cfg.tax;
	this->ante = cfg.ante;
	LOGGER(E_LOG_INFO) << "Ante=" << this->ante << " tax=" << this->tax;
}

void Table::setRoundTaskConf()
{
	int i = 0;
	for(i= 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player)
		{
			this->m_nRoundNum1 = player->coincfg.roundnum & 0x00FF;
			this->m_nRoundNum2 = player->coincfg.roundnum>>8 & 0x00FF;
			this->m_nRoundNum3 = player->coincfg.roundnum>>16 & 0x00FF;

			this->m_nGetCoinHigh1 = player->coincfg.coinhigh1;
			this->m_nGetCoinLow1 = player->coincfg.coinlow1;
			this->m_nGetCoinHigh2 = player->coincfg.coinhigh2;
			this->m_nGetCoinLow2 = player->coincfg.coinlow2;
			this->m_nGetCoinHigh3 = player->coincfg.coinhigh3;
			this->m_nGetCoinLow3 = player->coincfg.coinlow3;
			this->m_bRewardType = player->coincfg.rewardtype;
			this->m_nGetRollHigh1 = player->coincfg.rollhigh1;
			this->m_nGetRollLow1 = player->coincfg.rolllow1;
			this->m_nGetRollHigh2 = player->coincfg.rollhigh2;
			this->m_nGetRollLow2 = player->coincfg.rolllow2;
			this->m_nGetRollHigh3 = player->coincfg.rollhigh3;
			this->m_nGetRollLow3 = player->coincfg.rolllow3;
			this->m_nMagicCoin = player->coincfg.magiccoin;
		}
	}

	if(rand()%100 < Configure::getInstance()->rewardRate)
		this->m_bRewardType = 1;
	else
		this->m_bRewardType = 2;
}

//是否只有一个人正在玩牌的人，如果只有一个winner就为那个人
bool Table::iscanGameOver(Player **winner)
{
	if(!this->isActive())
		return false;
	short activeCount = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		//此人必须是active状态，并且有手牌
		if(player && player->isActive() && player->hascard)
		{
			++activeCount;
			*winner = player;
			if(activeCount > 1)
				return false;
		}
	}
	return true;
}

//是否只有一个人正在玩牌的人，如果只有一个winner就为那个人
bool Table::iscanGameOver()
{
	if(!this->isActive())
		return false;
	short activeCount = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		//此人必须是active状态，并且有手牌
		if(player && player->isActive() && player->hascard)
		{
			++activeCount;
			if(activeCount > 1)
				return false;
		}
	}
	return true;
}

void Table::setPlayerlimitcoin(Player* nextplayer)
{
	_LOG_DEBUG_("=-----------------maxbetcoin:%d\n", this->maxbetcoin);
	nextplayer->nextlimitcoin = this->maxbetcoin - nextplayer->betCoinList[this->currRound];
	if(nextplayer->nextlimitcoin > nextplayer->carrycoin - nextplayer->betCoinList[0])
		nextplayer->nextlimitcoin = nextplayer->carrycoin - nextplayer->betCoinList[0];
	else
		nextplayer->nextlimitcoin = nextplayer->nextlimitcoin;
}

void Table::setPlayerOptype(Player* player, short playeroptype)
{
	if(playeroptype == OP_CHECK)
	{
		if(this->currRound < 2)
			player->optype = OP_RASE|OP_CHECK|OP_THROW;
		else
			player->optype = OP_ALLIN|OP_RASE|OP_CHECK|OP_THROW;
	}
	else if (playeroptype == OP_CALL || playeroptype == OP_RASE)
	{
		if(this->currRound < 2)
			player->optype = OP_RASE|OP_CALL|OP_THROW;
		else
			player->optype = OP_ALLIN|OP_RASE|OP_CALL|OP_THROW;
	}
	else if (playeroptype == OP_ALLIN)
	{
		if(this->currRound < 2)
			player->optype = OP_RASE|OP_CALL|OP_THROW;
		else
			player->optype = OP_ALLIN|OP_RASE|OP_CALL|OP_THROW;
	}
	else if (playeroptype == OP_THROW)
	{
		if(this->currMaxCoin != 0)
		{
			if(this->currRound < 2)
				player->optype = OP_RASE|OP_CALL|OP_THROW;
			else
				player->optype = OP_ALLIN|OP_RASE|OP_CALL|OP_THROW;
		}
		else
		{
			if(this->currRound < 2)
				player->optype = OP_CHECK|OP_RASE|OP_THROW;
			else
				player->optype = OP_CHECK|OP_ALLIN|OP_RASE|OP_THROW;
		}
	}

	if(this->currMaxCoin == this->maxbetcoin)
		//当已经到达下注上限则直接把加注的操作类型给去除
		player->optype &= 0xFD;

	if((player->carrycoin - player->betCoinList[0] + player->betCoinList[this->currRound]) < this->currMaxCoin)
	{
		//当自己身上携带的钱已经不足以跟注的钱了，则只有allin和弃牌选项
		player->optype = OP_ALLIN|OP_THROW;
	}
}

Player* Table::getNextBetPlayer(Player* player, short playeroptype)
{
	if(player == NULL)
		return NULL;
	//说明此用户已经有操作行为
	if(this->currRound != 1)
		player->timeoutCount = 0;
	//当前这轮此用户已经下注
	player->thisroundhasbet = true;
	for(int i = 1; i < GAME_PLAYER; ++i)
	{
		int player_index = (player->tab_index+i)%GAME_PLAYER;
		Player* otherplayer = this->player_array[player_index];
		if(otherplayer && otherplayer->id != player->id && otherplayer->isActive() && otherplayer->hascard)
		{
			//当前已经轮到的用户就是加注用户
			if(otherplayer->id == raseUid)
				return NULL;
			//是第五轮则说明是在给用户开牌和弃牌两种选项
			if(this->currRound == 5)
			{
				//当前这个用户这轮已经选择则继续判断下一个用户
				if(otherplayer->thisroundhasbet)
					continue;
				betUid = otherplayer->id;
				otherplayer->optype = OP_THROW | OP_CHECK;
				return otherplayer;
			}
			//当前这轮都还没有下过注,并且没有allin
			if(!otherplayer->thisroundhasbet && !otherplayer->hasallin)
			{
				//设置当前用户为将要下注的用户
				betUid = otherplayer->id;
				setPlayerOptype(otherplayer, playeroptype);
				return otherplayer;
			}

			//说明下一个用户已经ALLIN了
			if(otherplayer->hasallin)
				continue;

			int nextplayerbetcoin = otherplayer->betCoinList[this->currRound];
			int thisplayerbetcoin = player->betCoinList[this->currRound];
			if((currMaxCoin == 0)&&(nextplayerbetcoin == thisplayerbetcoin))
				return NULL;

			//解决加注的人离开游戏，这轮一直不结束的bug
			if(otherplayer->thisroundhasbet && player->thisroundhasbet)
			{
				if(nextplayerbetcoin == thisplayerbetcoin && currMaxCoin == nextplayerbetcoin)
					return NULL;
			}
			
			//设置当前用户为将要下注的用户
			betUid = otherplayer->id;
			//if(otherplayer->carrycoin - otherplayer->betCoinList[0] + otherplayer->betCoinList[this->currRound] <= this->currMaxCoin)
			//{
			//	otherplayer->optype = OP_ALLIN|OP_THROW;
			//}
			//else
			//{
				setPlayerOptype(otherplayer, playeroptype);
			//}
			return otherplayer;
		}
	}
	return NULL;
}

short Table::getCanPlayNum()
{
	short num = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player && player->isActive() && player->hascard && !player->hasallin)
			num++;
	}
	return num;
}

int64_t Table::getSecondLargeCoin()
{
	int64_t firstcoin, secondcoin;
	firstcoin = secondcoin = 0;
	int i;
	int maxindex = -1;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player && player->isActive() && player->hascard)
		{
			if(player->carrycoin - player->betCoinList[0] > firstcoin)
			{
				maxindex = i;
				firstcoin = player->carrycoin - player->betCoinList[0];
				//_LOG_DEBUG_("id:%d firstcoin:%ld i:%d\n", player->id, firstcoin, i);
			}
		}
	}
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player && player->isActive() && player->hascard && i != maxindex)
		{
			if(player->carrycoin - player->betCoinList[0] > secondcoin)
			{
				secondcoin = player->carrycoin - player->betCoinList[0];
				//_LOG_DEBUG_("id:%d secondcoin:%ld i:%d\n", player->id, secondcoin, i);
			}
		}
	}
	return secondcoin;
}

void Table::setMaxBetCoin()
{
	int64_t secondLargeCoin = getSecondLargeCoin();
	//_LOG_DEBUG_("=======secondLargeCoin:%d=======\n", secondLargeCoin);
	//设置第一轮下注的最大数值
	if(this->currRound == 0)
	{
		int64_t confcoin= Configure::getInstance()->maxmulone * this->ante;
		this->maxbetcoin = confcoin < secondLargeCoin ? confcoin : secondLargeCoin;
	}
	//当是第一轮下注完毕，则第二轮是按照第一轮下注的数量定第二轮下注上限
	/*else if(this->currRound == 1)
	{
		int64_t confcoin= (Configure::getInstance()->maxmulone + Configure::getInstance()->maxmultwo)* this->ante - this->currMaxCoin;
		this->maxbetcoin = confcoin < secondLargeCoin ? confcoin : secondLargeCoin;
	}*/
	//否则没有上限
	else
	{
		this->maxbetcoin = secondLargeCoin;
	}
}

Player*  Table::getPreWinner()
{
	Player* prewinner = NULL;
	//如果是第五轮则设置前一个赢牌用户为牌面上的四个明牌大的人
	if(this->currRound == 5)
		prewinner = this->deduceWiner(1,4);
	else if(this->currRound == 0)
		prewinner = this->deduceWiner(1,1);
	else
		prewinner = this->deduceWiner(1,this->currRound);
	return prewinner;
}

Player* Table::getCurrWinner()
{
	Player* winner = NULL;
	//第六轮才是亮底牌的时候
	if(this->currRound == 6)
	{
		winner = this->deduceWiner(0,4);
	}
	else
	{
		if(this->currRound == 5)
			winner = this->deduceWiner(1,4);
		else
			winner = this->deduceWiner(1,this->currRound);
	}
	return winner;
}

Player* Table::getFirstBetPlayer(Player* winner)
{
	Player* firstBetPlayer = NULL;
	if(this->currRound < 5)
	{
		//如果当前最大牌的那个人已经allin了
		if(winner->hasallin)
		{
			for(int j = 1; j < GAME_PLAYER; ++j)
			{
				short nextindex = (winner->tab_index + j) % GAME_PLAYER;
				if(player_array[nextindex] && player_array[nextindex]->isActive() 
					&& !player_array[nextindex]->hasallin && player_array[nextindex]->hascard)
				{
					firstBetPlayer = player_array[nextindex];
					break;
				}
			}
		}

		if(firstBetPlayer == NULL)
			firstBetPlayer = winner;
		//前两轮用户可以选择加注但是不能梭哈
		if(this->currRound <= 1)
			firstBetPlayer->optype = OP_RASE|OP_CHECK|OP_THROW;
		else
			firstBetPlayer->optype = OP_RASE|OP_ALLIN|OP_CHECK|OP_THROW;
	}
	else if(this->currRound == 5)
	{
		firstBetPlayer = winner;
		firstBetPlayer->optype = OP_THROW|OP_CHECK;
	}
	else
	{
		firstBetPlayer = winner;
		firstBetPlayer->optype = 0;
	}
	return firstBetPlayer;
}

bool Table::hasSomeOneAllin()
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = this->player_array[i];
		if(player && player->isActive() && player->hascard && player->hasallin)
			return true;
	}
	return false;
}

int Table::setNextRound()
{
	this->stopBetCoinTimer();
	if(!hassendcard && this->currRound > 1)
	{
		hassendcard = true;
		this->startSendCardTimer(1500);
		return 0;
	}
	hassendcard = false;

	this->setMaxBetCoin();
	Player* prewinner = getPreWinner();

	this->currRound++;
	Player* winner = getCurrWinner();
	if(winner == NULL)
	{
		_LOG_ERROR_("Can't Get Winner tid[%d]\n",this->id);
		return -1;
	}
	Player* firstBetPlayer = getFirstBetPlayer(winner);

	//得到较小的那个能下注的数目
	setPlayerlimitcoin(firstBetPlayer);
	
	short activeCount = this->getCanPlayNum(); 
	//如果可以下注的人只有一个人的时候这个桌子的操作类型为0
	if(activeCount < 2 && this->currRound < 5)
		firstBetPlayer->optype = 0;

	this->startBetCoinTimer(firstBetPlayer->id, Configure::getInstance()->betcointime);
	firstBetPlayer->setBetCoinTime(time(NULL));
	//如果当前是第五轮了而第四轮大家都是看牌过来的，则直接跳过第五轮，转到第六轮结束游戏
	if(this->currRound == 5 && this->currMaxCoin == 0 && !this->hasSomeOneAllin())
	{
		this->currRound++;
		firstBetPlayer->optype = 0;
	}
	//当前加注的用户和下注的用户都设置为第一轮开始下注的人
	this->raseUid = this->betUid = firstBetPlayer->id;
	this->currMaxCoin = 0;
	int sendNum = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendNum == this->countPlayer)
			break;
		Player* getplayer = this->player_array[i];
		if(getplayer)
		{	
			getplayer->thisroundhasbet = false;//发牌的时候把所用用户当前轮是否下注置为否
			IProcess::sendRoundCard(getplayer, this, firstBetPlayer, prewinner, this->currRound);
			sendNum++;
		}
	}

	if(this->currRound == 6)
		return IProcess::GameOver(this, winner);
	
	//当第一个下注用户操作类型为空的时候直接发下一轮的牌
	if(firstBetPlayer->optype == 0)
		return setNextRound();
}

short Table::GetPlayerCardKind(BYTE bPlayerCardList[], BYTE bBeginPos, BYTE bEndPos)
{
	BYTE bPlayerCardArray[5];
	memcpy(bPlayerCardArray,bPlayerCardList,sizeof(bPlayerCardList));
	m_GameLogic.SortCard(bPlayerCardArray + bBeginPos,bEndPos - bBeginPos + 1);
	return m_GameLogic.GetCardKind(bPlayerCardArray + bBeginPos,bEndPos - bBeginPos + 1);
}

//推断胜者
Player* Table::deduceWiner(BYTE bBeginPos, BYTE bEndPos)
{
	if(this->countPlayer <= 0)
		return NULL;
	//保存扑克
	BYTE bTableCardArray[GAME_PLAYER][5];
	memcpy(bTableCardArray,m_bTableCardArray,sizeof(m_bTableCardArray));
	
	BYTE bWiner = 0;
	//寻找玩家
	for (bWiner=0;bWiner<GAME_PLAYER;bWiner++)
	{
		Player* player = player_array[bWiner];
		if (player && player->hascard) break;
	}

	//对比玩家
	for (BYTE i=(bWiner+1);i<GAME_PLAYER;i++)
	{
		Player* player = player_array[i];
		if (player == NULL) continue;
		if (!player->hascard) continue;
		if (m_GameLogic.CompareCard(bTableCardArray[i]+bBeginPos,bTableCardArray[bWiner]+bBeginPos,bEndPos-bBeginPos+1)==true) 
			bWiner=i;
	}

	//说明所有人手上都没有牌了
	if(bWiner == GAME_PLAYER)
		return NULL;
	return player_array[bWiner];
}

//推断胜者的顺序，用于传回给前端
Player* Table::deduceWinerOrder(BYTE bBeginPos, BYTE bEndPos)
{
	if(this->countPlayer <= 0)
		return NULL;
	//保存扑克
	BYTE bTableCardArray[GAME_PLAYER][5];
	memcpy(bTableCardArray,m_bTableCardArray,sizeof(m_bTableCardArray));
	
	BYTE bWiner = 0;
	//寻找玩家
	for (bWiner=0;bWiner<GAME_PLAYER;bWiner++)
	{
		Player* player = player_array[bWiner];
		//是否已经有序了
		if (player && player->hascard && !player->isorder) break;
	}

	//对比玩家
	for (BYTE i=(bWiner+1);i<GAME_PLAYER;i++)
	{
		Player* player = player_array[i];
		if (player == NULL) continue;
		if (!player->hascard) continue;
		//是否已经有序了
		if (player->isorder) continue;
		if (m_GameLogic.CompareCard(bTableCardArray[i]+bBeginPos,bTableCardArray[bWiner]+bBeginPos,bEndPos-bBeginPos+1)==true) 
			bWiner=i;
	}
	return player_array[bWiner];
}

int Table::gameOver(Player* winner, bool bisthrowwin)
{
	if(winner == NULL)
	{
		_LOG_ERROR_("[gameOver] winner id is NULL\n");
		return -1;
	}
	//判断是否是对手都弃牌了他赢得这局
	if(bisthrowwin)
		this->isthrowwin = true;
	this->stopBetCoinTimer();
	//设置上一盘赢牌的用户
	pregame_winner = winner;
	int i = 0;

	this->calcWinnerOrder();
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = this->player_array[i];
		//说明这个人还在桌子里面，只是弃牌了而已,把其加入赢钱列表
		if(player && player->isActive() && !player->hascard)
		{
			winnerorder[winnercount++] = player->id;
		}
	}

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = this->player_array[i];
		if(player && player->isActive())
		{
			player->status = STATUS_PLAYER_OVER;
			AllocSvrConnect::getInstance()->userUpdateStatus(player, STATUS_PLAYER_OVER);
			player->startnum++;
			//结束的时候把此用户最终手牌类型算出来
			if(player->hascard)
			{
				//只有真的是走到第六轮的时候才会计算此用户的最终牌型
				if(this->currRound == 6)
					player->finalcardvalue = this->GetPlayerCardKind(player->card_array, 0, 4);
				else
					player->finalcardvalue = -1;
			}
		}
	}
	this->status = STATUS_TABLE_OVER;
	this->setEndTime(time(NULL));

	AllocSvrConnect::getInstance()->updateTableStatus(this);
	this->calcCoin(winner);

	_LOG_INFO_("[gameOver] gameid[%s]\n", this->getGameID());

	this->startKickTimer(Configure::getInstance()->kicktime);
	return 0;
}

void Table::calcWinnerOrder()
{
	int i = 0;
	while(1)
	{
		bool iscanbreak = true;
		for(i = 0; i< GAME_PLAYER; ++i)
		{
			Player* player = this->player_array[i];
			if(player && player->hascard && !player->isorder)
			{
				iscanbreak = false;
				break;
			}
		}
		if(iscanbreak) break;
		Player *winner = deduceWinerOrder(0,4);
		winner->isorder = true;
		winnerorder[winnercount++] = winner->id;
	}
}

int Table::calcCoin(Player *gamewinner)
{
	int64_t tempLeveCoin = this->commonCoin;
	//离开输牌的金币数给赢牌的那一个人
	if(gamewinner->carrycoin >= tempLeveCoin)
	{
		gamewinner->finalgetCoin += this->commonCoin;
		tempLeveCoin = 0;
	}
	else
	{
		gamewinner->finalgetCoin += gamewinner->carrycoin;
		tempLeveCoin -= gamewinner->carrycoin;
	}

	int i = 0;
	while(1)
	{
		Player *winner = this->deduceWiner(0,4);
		if(winner == NULL)
			break;
		//前面已经给这个玩家加钱了，这里不能重复操作
		if(gamewinner->id != winner->id)
		{
			if(tempLeveCoin > 0)
			{
				if(winner->betCoinList[0] >= tempLeveCoin)
				{
					winner->finalgetCoin += tempLeveCoin;
					tempLeveCoin = 0;
				}
				else
				{
					winner->finalgetCoin += winner->betCoinList[0];
					tempLeveCoin -= winner->betCoinList[0];
				}
			}
		}
		
		for(i = 0; i < GAME_PLAYER; ++i)
		{
			Player* player = this->player_array[i];
			if(player && player->id != winner->id && player->betCoinList[0] != 0)
			{
				//结算完一个用户就把手牌设置为无状态
				if(player->betCoinList[0] <= winner->betCoinList[0])
				{
					winner->finalgetCoin += player->betCoinList[0];
					player->finalgetCoin -= player->betCoinList[0];
					player->betCoinList[0] = 0;
					//_LOG_DEBUG_("+++++winner:%d winnerbetcountcoin:%ld player:%d playerbetcountcoin:%ld winnerfinalgetcoin:%ld\n",
					//	winner->id, winner->betCoinList[0], player->id, player->betCoinList[0], winner->finalgetCoin);
					//当已经计算完此用户的金币则把其手牌也置为false
					player->hascard = false;
				}
				//说明此人金币没有完全被最大牌的用户赢光
				else
				{
					winner->finalgetCoin += winner->betCoinList[0];
					player->finalgetCoin -= winner->betCoinList[0];
					player->betCoinList[0] -= winner->betCoinList[0];
					//_LOG_DEBUG_("winner:%d winnerbetcountcoin:%ld player:%d playerbetcountcoin:%ld winnerfinalgetcoin:%ld\n",
					//	winner->id, winner->betCoinList[0], player->id, player->betCoinList[0], winner->finalgetCoin);
				}
			}
		}
		//把最大牌赢的用户也设置为没牌状态,并且把他下注的钱也清空
		winner->hascard = false;
		winner->betCoinList[0] = 0;
	}

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = this->player_array[i];
		if(player && player->finalgetCoin > 0)
		{
			Util::taxDeduction(player->finalgetCoin, this->tax, player->deducte_tax);
			if (E_MSG_SOURCE_ROBOT == player->source)
			{
				int64_t lRobotWin = player->finalgetCoin + player->deducte_tax;

				if (!OperationRedis::getInstance()->AddRobotWin(player->pid, Configure::getInstance()->server_id, (int)lRobotWin))
					LOGGER(E_LOG_DEBUG) << "OperationRedis::AddRobotWin Error, pid=" << player->pid << ", server_id=" << Configure::getInstance()->server_id << ", win=" << lRobotWin;
			}
			else
			{
				LOGGER(E_LOG_INFO) << "player = " << player->id << " is winner, cal tax now = " << this->tax
					<< " final get coin = " << player->finalgetCoin;

				if (!OperationRedis::getInstance()->UpdateTaxRank(player->pid, Configure::getInstance()->server_id, GAME_ID, Configure::getInstance()->level, player->tid, player->id, player->deducte_tax))
					LOGGER(E_LOG_DEBUG) << "OperationRedis::GenerateTip Error, pid=" << player->pid << ", server_id=" << Configure::getInstance()->server_id << ", gameid=" << GAME_ID << ", level=" << Configure::getInstance()->level << ", id=" << player->id << ", Tax=" << player->deducte_tax;
			}
		}
	}
	if (tempLeveCoin > 0)
	{
		_LOG_INFO_("Confiscated Coin tempLeveCoin[%ld] gameid[%s]\n", tempLeveCoin, this->getGameID());
	}

	return 0;
}

void Table::reSetInfo()
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = this->player_array[i];
		if(player)
			player->reset();
	}
	this->reset();
}

void Table::setKickTimer()
{
	if(this->isActive() || this->countPlayer <= 1)
		return;
	this->startKickTimer(Configure::getInstance()->kicktime);
	this->lockTable();
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = this->player_array[i];
		if(player && (!player->isActive() || !player->isReady())&& player->startnum == 1)
		{
			IProcess::serverWarnPlayerKick(this, player, Configure::getInstance()->kicktime - 2);
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



