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
	int i = 0;
	for(i = 0; i < LEVEL1NUM; ++i)
		level1id[i].uid = i + 701;
	for(i = 0; i < LEVEL2NUM; ++i)
		level2id[i].uid = i + 301;
	for(i = 0; i < LEVEL3NUM; ++i)
		level3id[i].uid = i + 501;
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
		_LOG_INFO_("Robot Exit uid[%d] playersize[%d]\n", player->id, playerMap.size());
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
	int randnum = rand()%200;
	if (Configure::getInstance()->level == 1)
	{
		for(int i = 0; i< LEVEL1NUM; i++)
		{
			int j = (randnum + i) % 200;
			if(!level1id[j].hasused)
			{
				level1id[j].hasused = true;
				return level1id[j].uid;
			}
		}
		return -1;
	}

	if (Configure::getInstance()->level == 2)
	{
		for(int i = 0; i< LEVEL2NUM; i++)
		{
			int j = (randnum + i) % 200;
			if(!level2id[j].hasused)
			{
				level2id[j].hasused = true;
				return level2id[j].uid;
			}
		}
		return -1;
	}

	if (Configure::getInstance()->level == 3)
	{
		for(int i = 0; i< LEVEL3NUM; i++)
		{
			int j = (randnum + i) % 200;
			if(!level3id[j].hasused)
			{
				level3id[j].hasused = true;
				return level3id[j].uid;
			}
		}
	}
	return -1;
}

int PlayerManager::delRobotUid(int robotuid)
{
	if(Configure::getInstance()->level == 1)
	{
		for(int i = 0; i< LEVEL1NUM; i++)
		{
			if(level1id[i].uid == robotuid)
			{
				level1id[i].hasused = false;
				return 0;
			}
		}
		return 0;
	}

	if (Configure::getInstance()->level == 2)
	{
		for(int i = 0; i< LEVEL2NUM; i++)
		{
			if(level2id[i].uid == robotuid)
			{
				level2id[i].hasused = false;
				return 0;
			}
		}
		return 0;
	}

	if (Configure::getInstance()->level == 3)
	{
		for(int i = 0; i< LEVEL3NUM; i++)
		{
			if(level3id[i].uid == robotuid)
			{
				level3id[i].hasused = false;
				return 0;
			}
		}
	}
	return 0;
}

int PlayerManager::ProcessOnTimerOut()
{
	this->StartTimer(1000 * 60);
	_LOG_INFO_("Check EveryActive Player, current Count player count=[%d]\n", playerMap.size());
	CoinConf::getInstance()->getCoinCfg(&cfg);
    return 0;
}




