#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <Game/GameUtil.h>
#include "Table.h"
#include "Logger.h"
#include "IProcess.h"
#include "Configure.h"
#include "AllocSvrConnect.h"
#include "CoinConf.h"
#include "HallManager.h"
#include "Util.h"
#include "Protocol.h"
#include "GameApp.h"
#include "StrFunc.h"
#include "RedisLogic.h"

bool compare_nocase (Player* first,Player* second)
{
	return (first)->m_lMoney > (second)->m_lMoney;
}

static std::string game_name = "BullHundred";

Table::Table():m_nStatus(-1)
{	 
	reloadCfg();
}
Table::~Table()
{
}

void Table::init()
{
	timer.init(this);
	this->id = Configure::getInstance()->m_nServerId << 16 | this->id;
	this->m_nStatus = STATUS_TABLE_IDLE;
	memset(player_array, 0, sizeof(player_array));
	m_nType = Configure::getInstance()->m_nLevel;
	PlayerList.clear();
	BankerList.clear();
	memset(m_lTabBetArray, 0, sizeof(m_lTabBetArray));
	memset(m_lRealBetArray, 0, sizeof(m_lRealBetArray));
	m_lTabBetLimit = 0;
	m_bHasBet = false;

	maxwinner = NULL;
	m_nLimitChatCoin = 0;
	m_nOneChatCost = 0;
	m_lBankerWinScoreCount = 0;
	this->startIdleTimer(Configure::getInstance()->idletime);
	this->setStatusTime(time(NULL));
	memset(m_CardResult, 0, sizeof(m_CardResult));
	m_nRecordLast=0;
	memset(m_GameRecordArrary, 0, sizeof(m_GameRecordArrary));
	m_nDespatchNum = 0;

	bankeruid = 0;
	bankernum = 0;
	m_nLimitCoin = 0;
	bankersid = -1;
	betVector.clear();
	reloadCfg();
	m_vecBlacklist.clear();
	receive_push_card_type.clear();

	for (int i = 0; i < MAX_SEAT_NUM; ++i)
	{
		m_SeatidArray[i] = NULL;
	}
}

void Table::reset()
{
	memset(m_lTabBetArray, 0, sizeof(m_lTabBetArray));
	memset(m_lRealBetArray, 0, sizeof(m_lRealBetArray));
	m_lTabBetLimit = 0;
	m_bHasBet = false;
	m_lBankerWinScoreCount = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player)
			player->reset();
	}
	memset(m_CardResult, 0, sizeof(m_CardResult));
	betVector.clear();
	maxwinner = NULL;
	m_nDespatchNum = 0;
	m_vecBlacklist.clear();
	receive_push_card_type.clear();
}

void Table::setStartTime(time_t t) 
{
	StartTime = t;
	bool bRet = Server()->HSETi(StrFormatA("%s:%d",game_name.c_str(),Configure::getInstance()->m_nServerId).c_str(),
									   "starttime", t);
	if (!bRet)
	{
		LOGGER(E_LOG_ERROR) << "save time to redis failed!";
	}
}

Player* Table::isUserInTab(int uid)
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = player_array[i];
		if(player && player->id == uid)
			return player;
	}
	return NULL;
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

bool Table::isAllRobot()
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player_array[i] && player_array[i]->source != 30 && player_array[i]->m_lBetArray[0] > 0)
		{
			return false;
		}
	}
	return true;
}

void Table::setToPlayerList(Player* player)
{
	if(player == NULL)
		return ;
	if (PlayerList.size() <= 0)
	{
		PlayerList.push_back(player);
		return;
	}
	list<Player*>::iterator it;
	it = PlayerList.begin();
	while(it != PlayerList.end())
	{
		Player* other = *it;
		if(other)
		{
			if(other->m_lMoney <= player->m_lMoney)
			{
				PlayerList.insert(it, player);
				return ;
			}
		}
		it++;
	}
	PlayerList.push_back(player);
}

void Table::setToBankerList(Player* player)
{
	if(player == NULL)
		return ;
	/*if (BankerList.size() <= 0)
	{
		BankerList.push_back(player);
		return;
	}
	list<Player*>::iterator it;
	it = BankerList.begin();
	while(it != BankerList.end())
	{
		Player* other = *it;
		if(other)
		{
			if(other->m_lMoney <= player->m_lMoney)
			{
				BankerList.insert(it, player);
				return ;
			}
		}
		it++;
	}*/
	BankerList.push_back(player);
}

void Table::CancleBanker(Player* player)
{
	if(player == NULL)
		return ;
	Player* noer = NULL;
	if(player->isbankerlist)
	{
		list<Player*>::iterator it;
		it = BankerList.begin();
		while(it != BankerList.end())
		{
			noer = *it;
			if(noer->id == player->id)
			{
				BankerList.erase(it);
				break;
			}
			it++;
		}
	}
	player->isbankerlist = false;
}

void Table::HandleBankeList()
{
	list<Player*>::iterator it;
	it = BankerList.begin();
	while(it != BankerList.end())
	{
		Player* other = *it;
		if(other)
		{
			if(other->m_lMoney < CoinConf::getInstance()->getCoinCfg()->bankerlimit)
			{
				other->isbankerlist = false;
				IProcess::NoteBankerLeave(this, other);
				BankerList.erase(it++);
			}
			else
				it++;
		}
		else
			BankerList.erase(it++);
	}

	//上庄列表排序
	//BankerList.sort(compare_nocase);
}

void Table::setBetLimit(Player* banker)
{
	if (banker == NULL)
	{
		LOGGER(E_LOG_ERROR) << "banker == NULL!";
		return;
	}
	static const int factor = 40;
	m_lTabBetLimit = banker->m_lMoney / factor;
	LOGGER(E_LOG_INFO) << "table area bet limit = " << m_lTabBetLimit;
}

void Table::rotateBanker()
{
	Player* prebanker = NULL;
	if(this->bankersid >=0)
		prebanker = this->player_array[this->bankersid];
	if(prebanker)
	{
		prebanker->isbankerlist = false;
		prebanker->m_nLastBetTime = time(NULL);
	}
	bankersid = -1;
	bankeruid = 0;//做庄的uid
	bankernum = 0;//连续做庄的次数

	list<Player*>::iterator it;
	it = BankerList.begin();
	_LOG_DEBUG_("bankersize:%d\n", BankerList.size());
	int i = 0;
	while(it != BankerList.end())
	{
		Player* player = *it;
		if(player)
			_LOG_DEBUG_("index:%d player[%d] seatid[%d]\n", i, player->id, player->m_nSeatID);
		it++;
		i++;
	}

	
	it = BankerList.begin();

	while (it != BankerList.end())
	{
		Player* banker = *it;
		if(banker && prebanker)
		{
			if(!banker->isonline)
			{
				BankerList.erase(it++);
				continue;
			}
			if(banker->id == prebanker->id)
			{
				_LOG_ERROR_("bankerlist id[%d] m_nSeatID[%d] is prebanker id[%d] m_nSeatID[%d] \n", banker->id, banker->m_nSeatID, prebanker->id, prebanker->m_nSeatID);
				BankerList.erase(it++);
				continue;
			}
		}

		if(banker)
		{
			bankersid = banker->m_nSeatID;
			bankeruid = banker->id;//做庄的uid
			bankernum = 0;//连续做庄的次数

			time_t t;       
			time(&t);               
			char time_str[32]={0};
			struct tm* tp= localtime(&t);
			strftime(time_str,32,"%Y%m%d%H%M%S",tp);
			char gameId[64] ={0};
			//Player* banker = NULL;
			//if(this->bankersid >=0)
			//	banker = this->player_array[this->bankersid];
			sprintf(gameId, "%s|%d|%02d", time_str, banker ? banker->id: 0, this->bankernum + 1);
			this->setGameID(gameId);
			BankerList.erase(it++);

			IProcess::rotateBankerInfo(this, banker, prebanker);
			//如果上庄了，要去掉在桌子上
			if (banker->m_seatid != 0)
			{
				this->LeaveSeat(banker->m_seatid);
				this->NotifyPlayerSeatInfo();
			}
			this->setBetLimit(banker);
			return;
		}
		BankerList.erase(it++);
	}
	
	if(BankerList.size() == 0)
	{	
		IProcess::rotateBankerInfo(this, NULL, prebanker);
		return ;
	}

}

int Table::playerComming(Player* player)
{
	if(player == NULL)
		return -1;

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player_array[i] == NULL)
		{
			player_array[i] = player;
			player->m_nStatus = STATUS_PLAYER_INGAME;
			setToPlayerList(player);
			break;
		}
	}
	player->tid = this->id;
	player->enter();
	++this->m_nCountPlayer;
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

	list<Player*>::iterator it;
	it = PlayerList.begin();
	while(it != PlayerList.end())
	{
		Player* other = *it;
		if(other)
		{
			if(other->id == player->id)
			{
				PlayerList.erase(it);
				break ;
			}
		}
		it++;
	}

	if(player->isbankerlist)
	{
		it = BankerList.begin();
		while(it != BankerList.end())
		{
			Player* other = *it;
			if(other)
			{
				if(other->id == player->id)
				{
					BankerList.erase(it);
					break ;
				}
			}
			it++;
		}
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
	_LOG_WARN_("Player[%d] Leave\n", player->id);
	
	this->setSeatNULL(player);
	
	if (player->m_seatid != 0)
	{
		this->LeaveSeat(player->m_seatid);
		this->NotifyPlayerSeatInfo();
	}

	player->leave();
	player->init();
	//当前用户减一
	--this->m_nCountPlayer;
	
	AllocSvrConnect::getInstance()->updateTableUserCount(this);
}

int Table::playerBetCoin(Player* player, short bettype, int64_t betmoney)
{
	if(player == NULL)
		return -1;

	if(bettype <= 0 || bettype > 4)
		return -1;

	// 金额必须为指定的加注金额
	Cfg* coinCfg = CoinConf::getInstance()->getCoinCfg();
	if (coinCfg != NULL &&
		betmoney != coinCfg->raise1 && betmoney != coinCfg->raise2 && betmoney != coinCfg->raise3 && betmoney != coinCfg->raise4)
	{
		return -1;
	}

	player->m_lMoney -= betmoney;
	player->m_lBetArray[bettype] += betmoney;
	player->m_lBetArray[0] += betmoney;
	this->m_lTabBetArray[bettype] += betmoney;
	this->m_lTabBetArray[0] += betmoney;
	player->m_lReturnScore += betmoney;
	player->m_nLastBetTime = time(NULL);
	if(player->source != E_MSG_SOURCE_ROBOT)
	{
		this->m_lRealBetArray[bettype] += betmoney;
		SaveBetInfoToRedis(player, bettype);
		ULOGGER(E_LOG_INFO, player->id) << "bettype = " << bettype << "tabel bet = " << this->m_lTabBetArray[bettype];
		m_bHasBet = true;
	}
	return 0;
}

static std::string get_color_string(uint8_t color)
{
	switch (color)
	{
	case 0x00:
		return "方块";
	case 0x10:
		return "梅花";
	case 0x20:
		return "红桃";
	case 0x30:
		return "黑桃";
	}
	return "";
}

static std::string card_data_to_string(uint8_t* cards, size_t len)
{
	std::string tmp16;
	for (size_t i = 0; i < len; i++)
	{
		uint8_t color = GameLogic::GetCardColor(cards[i]);
		tmp16 += "[" + get_color_string(color) + "]" + TOHEX(cards[i]);
	}
	return tmp16;
}

bool Table::SetResult(int64_t bankerwincount, int64_t userbankerwincount)
{
	if (receive_push_card_type.empty() || receive_push_card_type.size() < BETNUM) //未收到php推送的牌型
	{
		m_GameLogic.RandCardList(m_bTableCardArray[0], sizeof(m_bTableCardArray) / sizeof(m_bTableCardArray[0][0]));
	}
	else //收到php推送的牌型
	{
		GameLogic::initCardList();
		for (int i = 0; i < BETNUM; i++)
		{
			m_GameLogic.RandSpecialCardList(m_bTableCardArray[i], BETNUM, receive_push_card_type[i]);
			LOGGER(E_LOG_INFO) << card_data_to_string(m_bTableCardArray[i], BETNUM);
		}
	}
	int i, j;
	for(i = 0; i < 5; i++)
	{
		m_CardResult[i].type = m_GameLogic.GetCardType(this->m_bTableCardArray[i], 5);
		m_CardResult[i].mul = m_GameLogic.GetTimes(this->m_bTableCardArray[i], 5);
	}

	for(j = 0; j < GAME_PLAYER; ++j)
	{
		Player *player = this->player_array[j];
		if (player==NULL) continue;

		player->m_lTempScore = 0;
	}
	Player* banker = NULL;
	if(bankersid >=0)
		banker = this->player_array[bankersid];

	if(banker == NULL)
	{
		LOGGER(E_LOG_ERROR) << "SetResult: banker is NULL bankersid=" << bankersid << " bankeruid=" << bankeruid;
		return true;
	}

	int64_t lSystemScoreCount = 0;
	BYTE BMul = this->m_CardResult[0].mul;
	for(i = 1; i < 5; ++i)
	{
		BYTE OtherMul = this->m_CardResult[i].mul;
		bool isBankerWin = true;
		if(BMul == OtherMul)
		{
			if(!this->m_GameLogic.CompareCard(this->m_bTableCardArray[0], this->m_bTableCardArray[i], 5, 1, 1))
			{
				isBankerWin = false;
			}
		}

		if(OtherMul > BMul)
			isBankerWin = false;

		for(j = 0; j < GAME_PLAYER; ++j)
		{
			Player *player = this->player_array[j];
			if (player==NULL) continue;

			if (player->id == this->bankeruid) continue;
			
			if(player->m_lBetArray[i] != 0)
			{
				if(isBankerWin)
				{
					banker->m_lTempScore += player->m_lBetArray[i] * BMul;
					player->m_lTempScore -= player->m_lBetArray[i] * BMul;
				}
				else
				{
					banker->m_lTempScore -= player->m_lBetArray[i] * OtherMul;
					player->m_lTempScore += player->m_lBetArray[i] * OtherMul;
				}
			}
		}
	}

	for(j = 0; j < GAME_PLAYER; ++j)
	{
		Player *player = this->player_array[j];
		if (player==NULL) continue;
		
		if(player->source == E_MSG_SOURCE_ROBOT)
		{
			lSystemScoreCount += player->m_lTempScore;
		}
	}

	Cfg* coincfg = CoinConf::getInstance()->getCoinCfg();
	//机器人当莊
	if(banker->source == E_MSG_SOURCE_ROBOT)
	{
		int64_t tempScore = lSystemScoreCount + bankerwincount;
		if (tempScore >= coincfg->lowerlimit && tempScore <= coincfg->upperlimit)
			return true;

		if(tempScore < coincfg->lowerlimit)
		{
			//系统应该赢钱
			if(lSystemScoreCount >= 0)
				return true;
			return false;
		}

		if(tempScore > coincfg->upperlimit)
		{
			//系统应该输钱
			if(lSystemScoreCount <= 0)
				return true;
			return false;
		}
	}
	//用户当莊
	else
	{
		int64_t tempScore = banker->m_lTempScore + userbankerwincount;
		int64_t tempSysScore = lSystemScoreCount + bankerwincount;
		//这里要保证系统不输
		if (tempSysScore >= coincfg->lowerlimit)
		{
			//玩家在控制范围内
			if(tempScore >= coincfg->bankerlowerlimit && tempScore <= coincfg->bankerupperlimit)
				return true;

			//说明用户已经赢了很多
			if(tempScore > coincfg->bankerupperlimit)
			{
				if (banker->m_lTempScore <= 0)
					return true;
			}

			if(tempScore < coincfg->bankerlowerlimit)
			{
				if(banker->m_lTempScore >= 0)
					return true;
			}
			return false;
		}
		else
		{
			if (lSystemScoreCount >= 0)
				return true;
			return false;
		}
	}
	return true;
}

bool Table::SysSetResult(short result)
{
	m_GameLogic.RandCardList(m_bTableCardArray[0], sizeof(m_bTableCardArray)/sizeof(m_bTableCardArray[0][0]));
	int i;
	for(i = 0; i < 5; i++)
	{
		m_CardResult[i].type = m_GameLogic.GetCardType(this->m_bTableCardArray[i], 5);
		m_CardResult[i].mul = m_GameLogic.GetTimes(this->m_bTableCardArray[i], 5);
	}

	if (result < 1 || result > 6)
		return true;

	if(result == 5)
	{
		for(i = 1; i < 5; ++i)
		{
			if(!this->m_GameLogic.CompareCard(this->m_bTableCardArray[0], this->m_bTableCardArray[i], 5, 1, 1))
			{
				return false;
			}
		}
		if(m_CardResult[0].mul < Configure::getInstance()->bankerwintype)
			return false;
		return true;
	}

	if(result == 6)
	{
		for(i = 1; i < 5; ++i)
		{
			if(this->m_GameLogic.CompareCard(this->m_bTableCardArray[0], this->m_bTableCardArray[i], 5, 1, 1))
			{
				return false;
			}
		}
		return true;
	}

	if(!this->m_GameLogic.CompareCard(this->m_bTableCardArray[0], this->m_bTableCardArray[result], 5, 1, 1))
	{
		if(m_CardResult[0].mul > Configure::getInstance()->bankerlosetype)
			return false;
		if(m_CardResult[result].mul < Configure::getInstance()->areawintype)
			return false;
		return true;
	}

	return false;
	
}

bool Table::SetLoserResult(Player* loser)
{
	if(loser == NULL)
		return false;

	m_GameLogic.RandCardList(m_bTableCardArray[0], sizeof(m_bTableCardArray)/sizeof(m_bTableCardArray[0][0]));
	int i, j;
	for(i = 0; i < 5; i++)
	{
		m_CardResult[i].type = m_GameLogic.GetCardType(this->m_bTableCardArray[i], 5);
		m_CardResult[i].mul = m_GameLogic.GetTimes(this->m_bTableCardArray[i], 5);
	}

	for(j = 0; j < GAME_PLAYER; ++j)
	{
		Player *player = this->player_array[j];
		if (player==NULL) continue;

		player->m_lTempScore = 0;
	}
	Player* banker = NULL;
	if(bankersid >=0)
		banker = this->player_array[bankersid];

	if(banker == NULL)
	{
		_LOG_ERROR_("SetLoserResult: banker is NULL bankersid:%d bankeruid:%d\n", bankersid, bankeruid);
		return true;
	}

	//int64_t lSystemScoreCount = 0;
	BYTE BMul = this->m_CardResult[0].mul;
	for(i = 1; i < 5; ++i)
	{
		BYTE OtherMul = this->m_CardResult[i].mul;
		bool isBankerWin = true;
		if(BMul == OtherMul)
		{
			if(!this->m_GameLogic.CompareCard(this->m_bTableCardArray[0], this->m_bTableCardArray[i], 5, 1, 1))
			{
				isBankerWin = false;
			}
		}

		if(OtherMul > BMul)
			isBankerWin = false;

		for(j = 0; j < GAME_PLAYER; ++j)
		{
			Player *player = this->player_array[j];
			if (player==NULL) continue;

			if (player->id == this->bankeruid) continue;
			
			if(player->m_lBetArray[i] != 0)
			{
				if(isBankerWin)
				{
					banker->m_lTempScore += player->m_lBetArray[i] * BMul;
					player->m_lTempScore -= player->m_lBetArray[i] * BMul;
				}
				else
				{
					banker->m_lTempScore -= player->m_lBetArray[i] * OtherMul;
					player->m_lTempScore += player->m_lBetArray[i] * OtherMul;
				}
			}
		}
	}

	int randnum = rand()%100;

	for(j = 0; j < GAME_PLAYER; ++j)
	{
		Player *player = this->player_array[j];
		if (player==NULL) continue;
		
		if(player->id == loser->id)
		{
			if(randnum < Configure::getInstance()->randlose)
			{
				if(player->m_lTempScore < 0)
					return true;
			}
			else
				return true;
		}
	}

	return false;
	
}

void Table::GameOver()
{
	m_nDespatchNum = 0;
	int64_t bankerwincount = 0;
	//int64_t playerwincount = 0;
	int64_t userbankerwincount = 0;
	CoinConf::getInstance()->getWinCount(bankerwincount, userbankerwincount);
	//short resultret = CoinConf::getInstance()->getSwtichTypeResult();
	//Player* Loser = this->getLoseUser();
	//int printresult = 0;
	while(true)
	{
// 		if(resultret > 0 && resultret < 7)
// 		{
// 			if(SysSetResult(resultret))
// 			{
// 				printresult = 1;
// 				break;
// 			}
// 		}
// 		else 
// 		if (Loser && Configure::getInstance()->randlose > 0 &&
// 			((Loser->id != bankeruid && Loser->m_lBetArray[0] >= Configure::getInstance()->switchbetmoney) || Loser->id == bankeruid))
// 		{
// 			if(SetLoserResult(Loser))
// 			{
// 				printresult = 2;
// 				break;
// 			}
// 		}
// 		else
//		{
			if(SetResult(bankerwincount, userbankerwincount))
				break;
//		}
		m_nDespatchNum++;
		if(m_nDespatchNum > 100)
			break;
	}

	LOGGER(E_LOG_DEBUG) << "GameOver: GameID=" << this->getGameID() << " dispatch num=" << m_nDespatchNum <<
		" bet1=" << this->m_lTabBetArray[1] << " bet2=" << this->m_lTabBetArray[2] << " bet3=" << this->m_lTabBetArray[3]
		<< " bet4=" << this->m_lTabBetArray[4] << " bankeruid=" << bankeruid;
	CalculateScore();
	IProcess::GameOver(this);
	ClearBetInfo();
	receive_push_card_type.clear();
}

void Table::CalculateScore()
{
	int i,j; 

	for(i = 0; i < 5; i++)
	{
		BYTE index1,index2;
		index1 = index2 = 0;
		m_CardResult[i].type = m_GameLogic.GetCardType(this->m_bTableCardArray[i], 5, index1, index2);
		m_CardResult[i].mul = m_GameLogic.GetTimes(this->m_bTableCardArray[i], 5);
		BYTE bullcard[3];
		BYTE nobullcard[2];
		if(m_CardResult[i].type > 0 && m_CardResult[i].type < 20)
		{
			int m = 0;
			for(int n = 0; n < 5; n++)
			{
				if(n != index1 && n != index2)
					bullcard[m++] = this->m_bTableCardArray[i][n];
			}
			nobullcard[0] = this->m_bTableCardArray[i][index1];
			nobullcard[1] = this->m_bTableCardArray[i][index2];

			this->m_bTableCardArray[i][0] = bullcard[0];
			this->m_bTableCardArray[i][1] = bullcard[1];
			this->m_bTableCardArray[i][2] = bullcard[2];
			this->m_bTableCardArray[i][3] = nobullcard[0];
			this->m_bTableCardArray[i][4] = nobullcard[1];
		}

		char cardbuff[64]={0};
		for(j = 0; j < 5; ++j)
		{
			sprintf(cardbuff+5*j, "0x%02x ",this->m_bTableCardArray[i][j]);
		}
		LOGGER(E_LOG_DEBUG) << "GameID=" << this->getGameID() << " cardarray=" << cardbuff << " cardtype=" << m_CardResult[i].type
			<< " mul=" << m_CardResult[i].mul;
	}

	Player* banker = NULL;
	if (bankersid >=0)
		banker = this->player_array[bankersid];

	if(banker == NULL)
	{
		LOGGER(E_LOG_ERROR) << "banker is NULL bankersid=" << bankersid << " bankeruid=" << bankeruid;
		return;
	}

	//游戏记录
	ServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];

	BYTE BMul = this->m_CardResult[0].mul;
	for(i = 1; i < 5; ++i)
	{
		BYTE OtherMul = this->m_CardResult[i].mul;
		bool isBankerWin = true;
		if(BMul == OtherMul)
		{
			if(!this->m_GameLogic.CompareCard(this->m_bTableCardArray[0], this->m_bTableCardArray[i], 5, 1, 1))
			{
				isBankerWin = false;
			}
		}

		if(OtherMul > BMul)
			isBankerWin = false;

		if(isBankerWin)
		{
			if(i == 1)
				GameRecord.cbTian=20;
			if(i == 2)
				GameRecord.cbDi=20;
			if(i == 3)
				GameRecord.cbXuan=20;
			if(i == 4)
				GameRecord.cbHuang=20;
		}
		else
		{
			if(i == 1)
				GameRecord.cbTian=21;
			if(i == 2)
				GameRecord.cbDi=21;
			if(i == 3)
				GameRecord.cbXuan=21;
			if(i == 4)
				GameRecord.cbHuang=21;
		}

		if (isBankerWin)
		{
			banker->m_lWinScore += this->m_lTabBetArray[i] * BMul;
			banker->m_lResultArray[i] += this->m_lTabBetArray[i] * BMul;
			this->m_lBankerWinScoreCount += this->m_lTabBetArray[i] * BMul;
		}
		else
		{
			banker->m_lWinScore -= this->m_lTabBetArray[i] * OtherMul;
			banker->m_lResultArray[i] -= this->m_lTabBetArray[i] * OtherMul;
			this->m_lBankerWinScoreCount -= this->m_lTabBetArray[i] * OtherMul;
		}

		for(j = 0; j < GAME_PLAYER; ++j)
		{
			Player *player = this->player_array[j];
			if (player==NULL) continue;

			if (player->id == this->bankeruid) continue;
			
			if(player->m_lBetArray[i] != 0)
			{
				if(isBankerWin)
				{
					player->m_lWinScore -= player->m_lBetArray[i] * BMul;
					player->m_lResultArray[i] -= player->m_lBetArray[i] * BMul;
				}
				else
				{
					player->m_lWinScore += player->m_lBetArray[i] * OtherMul;
					player->m_lResultArray[i] += player->m_lBetArray[i] * OtherMul;
				}
			}
		}
	}

	if(banker->m_lWinScore >= 0)
	{
		GameRecord.cbBanker=21;
	}
	else
	{
		GameRecord.cbBanker=20;
	}
	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;

	int64_t lsysrobotcount = 0;
	for(j = 0; j < GAME_PLAYER; ++j)
	{
		Player *player = this->player_array[j];
		if (player==NULL) 
			continue;

		player->accumlatePlayCount(bankeruid);

		if (player->m_lWinScore > 0) {
			GameUtil::CalcSysWinMoney(player->m_lWinScore, player->tax, this->m_tax);

			if (E_MSG_SOURCE_ROBOT == player->source) {
				int64_t lRobotWin = player->m_lWinScore + player->tax;
				
				lsysrobotcount += lRobotWin;
				if (!RedisLogic::AddRobotWin(Tax(), player->pid, Configure::getInstance()->m_nServerId, (int) lRobotWin))
				{
					LOGGER(E_LOG_WARNING) << "OperationRedis::AddRobotWin Error, pid=" << player->pid
										<< ", m_nServerId=" << Configure::getInstance()->m_nServerId << ", win="
										<< lRobotWin;
				}
			} else {
				if (player->id == bankeruid)
					this->m_lBankerWinScoreCount -= player->tax;

				if (!RedisLogic::UpdateTaxRank(Tax(), player->pid, Configure::getInstance()->m_nServerId,
																  GAME_ID, Configure::getInstance()->m_nLevel,
																	player->tid, player->id, player->tax)) {
					LOGGER(E_LOG_WARNING) << "OperationRedis::GenerateTip Error, pid=" << player->pid
										  << ", m_nServerId=" << Configure::getInstance()->m_nServerId << ", gameid="
										  << GAME_ID << ", level=" << Configure::getInstance()->m_nLevel << ", id="
										  << player->id << ", Tax=" << player->tax;
				}
			}
		}
		else //机器人输钱也需要算
		{
			if (player->source == E_MSG_SOURCE_ROBOT)
			{
				lsysrobotcount += player->m_lWinScore;
			}
		}
				
		player->m_lMoney += player->m_lWinScore + player->m_lReturnScore;
	}

	int64_t bankerwincount = 0;
	if(banker->source != E_MSG_SOURCE_ROBOT) {
		bankerwincount = banker->m_lWinScore + banker->tax;
	}

	//暗扣
//	int discount = 0;
//	if (lsysrobotcount > 0)
//	{
//		GameUtil::getInstance().CalcSysWinMoney(lsysrobotcount, discount);
//		IProcess::SaveHundredDiscount(this, discount);
//	}
	LOGGER(E_LOG_INFO) << "id=" << this->id << " sysrobotcount =" << lsysrobotcount; //<< " discount=" << discount;

	CoinConf::getInstance()->setWinCount(lsysrobotcount, bankerwincount);
	++bankernum;
}

Player* Table::getLoseUser()
{
	if(m_vecBlacklist.size() <= 0)
		return NULL;
	int64_t maxBetCoin = 0;
	//int retuid = 0;
	Player* retplayer = NULL;
	Player* banker = NULL;
	if(bankersid >=0)
		banker = this->player_array[bankersid];

	if(banker != NULL)
	{
		vector<int>::iterator it = find(m_vecBlacklist.begin(), m_vecBlacklist.end(), banker->id);
		if (it != m_vecBlacklist.end())
			return banker;
	}

	for(int j = 0; j < GAME_PLAYER; ++j)
	{
		Player *player = this->player_array[j];
		if (player==NULL) continue;
		if (player->source == 30) continue;
		if (player->m_lBetArray[0] == 0) continue;
		vector<int>::iterator it = find(m_vecBlacklist.begin(), m_vecBlacklist.end(), player->id);
		if (it != m_vecBlacklist.end() && player->m_lBetArray[0] > maxBetCoin)
		{
			maxBetCoin = player->m_lBetArray[0];
			retplayer = player;
		}
	}
	return retplayer;
}


void Table::reloadCfg()
{
	CoinConf* coinCalc = CoinConf::getInstance();
	Cfg* coincfg = coinCalc->getCoinCfg();
	
	if (coincfg->tax > 100 || coincfg->tax < 0)
		coincfg->tax = 5;
	this->m_tax = coincfg->tax;
	this->m_nLimitCoin = coincfg->betlimitcoin;
	this->m_nRetainCoin = coincfg->retaincoin;
}

void Table::checkClearCfg()
{
	time_t t;       
    time(&t);               
    /*struct tm* tp= */localtime(&t);
}

bool Table::EnterSeat(int seatid, Player *player)
{
	if (seatid > MAX_SEAT_NUM || seatid == 0)
	{
		_LOG_ERROR_("Table::EnterSeat : Seatid=[%d] is overflow.\n", seatid);
		return false;
	}

	if (m_SeatidArray[seatid-1] != NULL) 
	{
		_LOG_ERROR_("Table::EnterSeat : Seatid=[%d] be seated uid=[%d].\n", seatid, m_SeatidArray[seatid-1]->id);
		return false;
	}

	if (player->m_seatid != 0) 
	{
		LeaveSeat(player->m_seatid);
	}


	player->m_seatid = seatid;
	m_SeatidArray[seatid - 1] = player;
	_LOG_INFO_("Table::EnterSeat : successed uid=[%d] Seatid=[%d] seated .\n", m_SeatidArray[seatid-1]->id, seatid);
	return true;
}

bool Table::LeaveSeat(int seatid)
{
	if (seatid > MAX_SEAT_NUM || seatid == 0)
	{
		_LOG_ERROR_("Table::LeaveSeat : Seatid=[%d] is overflow.\n", seatid);
		return false;
	}

	if (m_SeatidArray[seatid-1] != NULL)
	{
		_LOG_INFO_("Table::LeaveSeat : successed uid=[%d] Seatid=[%d] seated .\n", m_SeatidArray[seatid-1]->id, seatid);
		m_SeatidArray[seatid-1]->m_seatid = 0;
		m_SeatidArray[seatid-1] = NULL;
		return true;
	}
	return false;
}

void Table::SendSeatInfo(Player *player)
{
	_LOG_INFO_("Table::SendSeatInfo : player->uid=[%d]\n", player->id);
	if (player == NULL)
	{
		_LOG_ERROR_("Table::SendSeatInfo : why player is null.\n");
		return;
	}

	OutputPacket response;
	response.Begin(CLIENT_MSG_REFRESH_SEATLIST, player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteInt((Configure::getInstance()->m_nServerId << 16) | this->id);

	response.WriteInt(MAX_SEAT_NUM); //一共多少桌位号

	for (int i = 0; i < MAX_SEAT_NUM; ++i)
	{	
		if (m_SeatidArray[i] != NULL)
		{
			response.WriteInt(i+1);							//座位ID
			response.WriteInt(m_SeatidArray[i]->id);		//玩家ID
			response.WriteString(m_SeatidArray[i]->name);	//玩家名称
			response.WriteString(m_SeatidArray[i]->headlink);//玩家头像url
			response.WriteInt64(m_SeatidArray[i]->m_lMoney);					//玩家金币
		} else {
			response.WriteInt(i+1);      //座位ID
			response.WriteInt(0);		 //玩家ID
			response.WriteString("");	 //玩家名称
			response.WriteString("");	 //玩家头像url
			response.WriteInt64(0);		 //玩家金币
		}	
	}
	for (int i = 0; i < MAX_SEAT_NUM; ++i)
	{
		if (m_SeatidArray[i] != NULL)
		{
			response.WriteString(m_SeatidArray[i]->json);      //player json
		}
		else
		{
			response.WriteString("");      //player json
		}
	}
	response.End();
	HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
}

void Table::NotifyPlayerSeatInfo()
{
	_LOG_INFO_("Table::NotifyPlayerSeatInfo");
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if (this->player_array[i])
		{
			SendSeatInfo(this->player_array[i]);
		}
	}
}

void Table::SaveBetInfoToRedis(Player * player, int bettype)
{
	// 0 表示玩家下注总数
	// 1 天 2 地 3 玄 4 黄
	bool bRet = Server()->HSETi(StrFormatA("%s:%d:%d",game_name.c_str(),
                                           Configure::getInstance()->m_nServerId,
                                           bettype).c_str(),
                                StrFormatA("%d", player->id).c_str(),
                                player->m_lBetArray[bettype]);
	if (!bRet)
	{
		LOGGER(E_LOG_ERROR) << "save to redis failed!bettype = " << bettype << " betmoney = " << player->m_lBetArray[bettype];
	}
}

void Table::ClearBetInfo()
{
	for (int i = 1; i < BETNUM; i++)
	{
		bool bRet = Server()->DEL(StrFormatA("%s:%d:%d",
			game_name.c_str(),
			Configure::getInstance()->m_nServerId,
			i).c_str());
		if (!bRet)
		{
			LOGGER(E_LOG_ERROR) << "save to redis failed!bettype = " << i;
		}
	}
}

void Table::ReceivePushCardType(const std::vector<BYTE> cardtypes)
{
	receive_push_card_type = cardtypes;
}

//==================================TableTimer=================================================
void Table::startIdleTimer(int timeout)
{
	timer.startIdleTimer(timeout);
}

void Table::stopIdleTimer()
{
	timer.stopIdleTimer();
}

void Table::startBetTimer(int timeout)
{
	timer.startBetTimer(timeout);
}

void Table::stopBetTimer()
{
	timer.stopBetTimer();
}

void Table::startOpenTimer(int timeout)
{
	timer.startOpenTimer(timeout);
}

void Table::stopOpenTimer()
{
	timer.stopOpenTimer();
}


