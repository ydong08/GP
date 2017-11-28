#include <stdlib.h>
#include <string.h>
#include <map>
#include "GameCmd.h"
#include "PlayerManager.h"
#include "Logger.h"
#include "Configure.h"
#include "HallHandler.h"
#include "CoinConf.h"
#include "NamePool.h"
#include "Util.h"
#include "Protocol.h"

using namespace std;
static PlayerManager* instance = NULL;
PlayerManager* PlayerManager::getInstance()
{
	if(instance==NULL)
	{
		instance = new PlayerManager();
	}
	return instance;
}

PlayerManager::PlayerManager()
{
	addbetrobot = false;
	int startUid = RobotUtil::getRobotStartUid(E_BULLFIGHT_GAME_ID, Configure::getInstance()->m_nLevel, Configure::getInstance()->m_nRobotIndex);
	LOGGER(E_LOG_INFO) << "Robot Start uid=" << startUid;
	for (int i = 0; i < MAX_ROBOT_NUMBER; ++i)
	{
		m_idpool[i].uid = startUid + i;
		m_idpool[i].hasused = false;
	}
	this->StartTimer(1000*60);
	CoinConf::getInstance()->getCoinCfg(&cfg);

	ResetBetArray();
}

int PlayerManager::init()
{
	return 0;
}

int PlayerManager::addPlayer(int uid, Player* player)
{
	playerMap[uid] = player;
	return 0;
}

int PlayerManager::getPlayerSize()
{
	return playerMap.size();
}

void PlayerManager::calculateAreaBetLimit(int bankeruid)
{
	Player* banker = getPlayer(bankeruid);
	if (banker != NULL)
	{
		static const int FACTOR = 40;
		int64_t areaLimit = banker->money / FACTOR;
		for (int i = 1; i < BETNUM; i++)
		{
			int rate = Util::rand_range(30, 50);
			areaTotalBetLimit[i] = areaLimit * rate / 100;
		}
	}
}

void PlayerManager::updateAreaBetLimit(int64_t bankermoney, int bettype, int rate)
{
	static const int FACTOR = 40;
	int64_t areaLimit = bankermoney / FACTOR;
	if (rate == 0)
	{
		rate = Util::rand_range(30, 50);
	}
	areaTotalBetLimit[bettype] = areaLimit * rate / 100;
}

void PlayerManager::ResetBetArray()
{
	memset(areaTotalBetLimit, 0, sizeof(areaTotalBetLimit));
	memset(areaTotalBetArray, 0, sizeof(areaTotalBetArray));
}

bool PlayerManager::CheckAreaBetLimit(int bettype, int64_t betcoin)
{
	assert(bettype < BETNUM);
	if (bettype >= BETNUM)
	{
		return false;
	}
	int64_t limit = areaTotalBetLimit[bettype];
	int64_t curr_total = areaTotalBetArray[bettype];
	return curr_total + betcoin <= limit;
}

int PlayerManager::SavePlayerBetRecord(int uid, int bettype, int64_t betmoney, int pc)
{
	int rate = 0;
	BetArea a;
	a.betmoney = betmoney;
	a.bettype = bettype;
	a.continuePlayCount = pc;
	auto rec = playerBetRecord.find(uid);
	if (rec == playerBetRecord.end())
	{
		if (pc == 0 || betmoney == 0)
		{
			return 0;
		}
		std::deque<BetArea> ba;
		ba.push_back(a);
		playerBetRecord[uid] = ba;
	}
	else
	{
		auto& ba = rec->second;
		if (pc == 0 || betmoney == 0)
		{
			ba.clear();
			return 0;
		}
		ba.push_back(a);
		if (ba.size() > 5)	//最多保存5个
		{
			ba.pop_front();
		}
		//条件1
		int rate1 = 0;
		if (pc == 1 && betmoney > 50000)
		{
			rate1 = 50;
		}
		if (pc == 2 && betmoney > 50000)
		{
			rate1 = 60;
		}
		if (pc >= 3 && betmoney > 50000)
		{
			rate1 = 70;
		}
		//条件2
		int rate2 = 0;
		if (ba.size() > 1 && ba[0].betmoney > 20000)
		{
			std::vector<int64_t> progression;
			for (size_t i = 1; i < ba.size(); i++)
			{
				progression.push_back(ba[i].betmoney - ba[i - 1].betmoney);
			}
			if (progression.size() == 1)
			{
				if (progression[0] > 10000)
				{
					rate2 = 50;
				}
			}
			else
			{
				bool increase = true;
				for (size_t i = 1; i < progression.size(); i++)
				{
					if (progression[i] <= progression[i - 1])
					{
						increase = false;
						break;
					}
				}
				if (increase && progression[0] > 10000)
				{
					if (progression.size() == 3)
					{
						rate2 = 60;
					}
					if (progression.size() >= 4)
					{
						rate2 = 70;
					}
				}
			}
		}
		if (rate2 > rate1)
		{
			rate = rate2;
		}
		else
		{
			rate = rate1;
		}
	}
	return rate;
}

void PlayerManager::ClearBetRecord()
{
	playerBetRecord.clear();
}

void PlayerManager::delPlayer(int uid,bool isOnclose)
{
	Player* player = getPlayer(uid);
	if(player)
	{
		if(isOnclose)
			player->handler = NULL;

		LOGGER(E_LOG_INFO)<<"Robot Exit uid["<<player->id<<"] playersize["<<playerMap.size()<<"]";
		delRobotUid(player->id);
		if(player->handler)
			delete player->handler;

		NamePool::getInstance()->FreeName(player->name);

		delete player;
		playerMap.erase(uid);
	}
}

Player* PlayerManager::getPlayer(int uid)
{
	map<int , Player*>::iterator it = playerMap.find(uid);
	if(it == playerMap.end())
		return NULL;
	else
		return it->second;
}

int PlayerManager::getRobotUid()
{
	for (int i = 0; i < MAX_ROBOT_NUMBER; ++i)
	{
		if (!m_idpool[i].hasused)
		{
			m_idpool[i].hasused = true;
			return m_idpool[i].uid;
		}
	}
	return -1;
}

int PlayerManager::delRobotUid(int robotuid)
{
	for (int i = 0; i < MAX_ROBOT_NUMBER; ++i)
	{
		if (m_idpool[i].uid == robotuid)
		{
			m_idpool[i].hasused = false;
			return 0;
		}
	}
	return 0;
}

int PlayerManager::ProcessOnTimerOut()
{
	this->StartTimer(1000 * 60);
	LOGGER(E_LOG_INFO)<<"Check EveryActive Player, current Count player count=["<<playerMap.size()<<"]";
	CoinConf::getInstance()->getCoinCfg(&cfg);
	return 0;
}




