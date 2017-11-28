#include <stdlib.h>
#include <string.h>
#include <map>
#include "GameCmd.h"
#include "PlayerManager.h"
#include "Logger.h"
#include "Options.h"
#include "HallHandler.h"
#include "Util.h"
#include "RobotDefine.h"

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
	int startUid = Util::getRobotStartUid(E_SHOWHAND_GAME_ID, Options::instance()->level, Options::instance()->m_robotId);
	LOGGER(E_LOG_INFO) << "Robot Start uid=" << startUid;

	int i = 0;
	for(i = 0; i < IDNUM; ++i)
		idbuff[i].uid = startUid + i;
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

void  PlayerManager::delPlayer(int uid)
{
	Player* player = getPlayer(uid);
	if(player)
	{
		LOGGER(E_LOG_INFO)<< "Robot Exit uid["<<player->id<<"]";
		delRobotUid(player->id);
		if (player->handler)
		{
			player->handler->CloseConnect();
			player->handler = NULL;
		}
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



