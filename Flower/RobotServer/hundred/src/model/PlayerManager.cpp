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
	int startUid = RobotUtil::getRobotStartUid(Configure::getInstance().m_nGameID, Configure::getInstance().m_nLevel, Configure::getInstance().m_nRobotIndex);
	LOGGER(E_LOG_INFO) << "Robot Start uid=" << startUid;
	for (int i = 0; i < MAX_ROBOT_NUMBER; ++i)
	{
		m_idpool[i].uid = startUid + i;
		m_idpool[i].hasused = false;
	}
	this->StartTimer(1000*60);
	CoinConf::getInstance()->getCoinCfg(&cfg);
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
	LOGGER(E_LOG_INFO)<<"Check EveryActive Player, current Count player count=["<<playerMap.size()<<"]";
	CoinConf::getInstance()->getCoinCfg(&cfg);
	this->StartTimer(1000 * 60);
	return 0;
}




