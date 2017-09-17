#include <stdlib.h>
#include <string.h>
#include <map>
#include "GameCmd.h"
#include "PlayerManager.h"
#include "Logger.h"
#include "Configure.h"
#include "HallHandler.h"
#include "NamePool.h"
#include "RobotUtil.h"

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
    int startID = RobotUtil::getRobotStartUid(Configure::instance()->m_nGameID, Configure::instance()->m_nLevel, Configure::instance()->m_nRobotIndex);
    LOGGER(E_LOG_INFO) << "Robot Start uid=" << startID;
    int i = 0;
    for(i = 0; i < IDNUM; ++i) {
        idbuff[i].hasused = false;
        idbuff[i].uid = startID + i;
    }
	m_playerMap.clear();
}

int PlayerManager::init()
{
	return 0;
}

int PlayerManager::addPlayer(int uid, Player* player)
{
	m_playerMap[uid] = player;
	return 0;
}

int PlayerManager::getPlayerSize()
{
	return m_playerMap.size();
}

void  PlayerManager::delPlayer(int uid)
{
	Player* player = getPlayer(uid);
	if(player)
	{
		LOGGER(E_LOG_INFO)<< "Robot Exit uid["<<player->id<<"]";
		delRobotUid(player->id);
		NamePool::getInstance()->FreeName(player->name);
		//if(player->handler)
		//	delete player->handler;
		map<int , Player*>::iterator it = m_playerMap.begin();

		while(it != m_playerMap.end())
		{
			Player* user=it->second;
			if(user)
				//_LOG_DEBUG_("first:%d uid:%d\n", it->first, user->id);
				LOGGER(E_LOG_DEBUG)<< "first:"<<it->first<<" uid:"<<user->id;
			it++;
		}

		delete player;

		m_playerMap.erase(uid);
	}
}

Player* PlayerManager::getPlayer(int uid)
{
	map<int , Player*>::iterator it = m_playerMap.find(uid);
	if(it == m_playerMap.end())
		return NULL;
	else
		return it->second;
}

int PlayerManager::getRobotUID()
{
	for(int i = 0; i< IDNUM; i++)
	{
		if(!idbuff[i].hasused)
		{
			idbuff[i].hasused = true;
			return idbuff[i].uid;
		}
	}
	return -1;
}

int PlayerManager::delRobotUid(int robotuid)
{
	for(int i = 0; i< IDNUM; i++)
	{
		if(idbuff[i].uid == robotuid)
		{
			idbuff[i].hasused = false;
			return 0;
		}
	}
	return 0;
}



