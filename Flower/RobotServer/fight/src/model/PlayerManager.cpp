#include <stdlib.h>
#include <string.h>
#include <map>
#include "GameCmd.h"
#include "PlayerManager.h"
#include "Logger.h"
#include "Despatch.h"
#include "HallHandler.h"
#include "Util.h"
#include "Configure.h"
#include "RobotDefine.h"
#include "Protocol.h"

using namespace std;

PlayerManager::PlayerManager()
{
	int startUid = RobotUtil::getRobotStartUid(E_FLOWER_GAME_ID, Configure::getInstance().m_robotId, Configure::getInstance().m_robotId);
	LOGGER(E_LOG_INFO) << "Robot Start uid=" << startUid;
	for (int i = 0; i < MAX_ROBOT_NUMBER; ++i)
	{
		m_idpool[i].uid = startUid + i;
		m_idpool[i].hasused = false;
	}
}

PlayerManager::~PlayerManager()
{

}

Player* PlayerManager::getPlayer(int uid)
{
	map<int, Player*>::iterator it = m_playerMap.find(uid);
	if (it == m_playerMap.end())
		return NULL;
	else
		return it->second;
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

void  PlayerManager::delPlayer(int uid, bool iscolsed)
{
	Player* player = getPlayer(uid);
	if (player)
	{
		LOGGER(E_LOG_INFO) << "Exit Robot id=" << player->id;
		delRobotUid(player->id);
		if (!iscolsed)
		{
			if (player->handler)
			{
				player->handler->CloseConnect();
				player->handler = NULL;
			}
		}
		delete player;
		m_playerMap.erase(uid);
	}
}

int PlayerManager::getRobotUID()
{
	for(int i = 0; i < MAX_ROBOT_NUMBER; i++)
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
	for(int i = 0; i< MAX_ROBOT_NUMBER; i++)
	{
		if (m_idpool[i].uid == robotuid)
		{
			m_idpool[i].hasused = false;
			return 0;
		}
	}
	return 0;
}

int PlayerManager::productRobot(int tid, short status, short level, short countUser)
{
	Player* player = new Player();
	if (player)
	{
		player->init();
		
		player->id = PlayerManager::getInstance().getRobotUID();
		if (player->id <= 0)
		{
			LOGGER(E_LOG_WARNING) << "robotuid is negative level=" << level << " robotuid=" << player->id;
			delete player;
			player = NULL;
			return -1;
		}
		int   totalnum = 0;
		int   winnum = 0;
		int   roll = 0;
		int   switchmoney = 0;
		short vip = 0;
		int   mostwin = 0;
		if (level == E_PRIMARY_LEVEL) 
		{
			player->money = Configure::getInstance().baseMoney1 + rand() % Configure::getInstance().randMoney1;
			totalnum += 5 + rand() % 6;
			winnum += 1 + rand() % 3;
			roll += 1;
			switchmoney = Configure::getInstance().swtichWin1;
			mostwin = 2000 + rand() % 30000;
		} 
		else if (level == E_MIDDLE_LEVEL)
		{
			player->money = Configure::getInstance().baseMoney2 + rand() % Configure::getInstance().randMoney2;
			totalnum = 50 + rand() % 800;
			winnum = totalnum * 4 / 10;
			roll += 150 + rand() % 100;
			switchmoney = Configure::getInstance().swtichWin2;
			vip = rand() % 100 < 30 ? 1 : 0;
			mostwin = 20000 + rand() % 300000;
		} 
		else if (level == E_ADVANCED_LEVEL) 
		{
			player->money = Configure::getInstance().baseMoney3 + rand() % Configure::getInstance().randMoney3;
			totalnum = 50 + rand() % 1000;
			winnum = totalnum * 4 / 10;
			roll += 250 + rand() % 200;
			switchmoney = Configure::getInstance().swtichWin3;
			vip = rand() % 100 < 50 ? 1 : 0;
			mostwin = 2000000 + rand() % 30000000;
		}
		else if (level == E_MASTER_LEVEL) 
		{
			player->money = Configure::getInstance().baseMoney4 + rand() % Configure::getInstance().randMoney4;
			totalnum = 50 + rand() % 3000;
			winnum = totalnum * 8 / 10;
			roll = 450 + rand() % 200;
			switchmoney = Configure::getInstance().swtichWin4;
			vip = rand() % 100 < 50 ? 1 : 0;
			mostwin = 2000000 + rand() % 30000000;
		}
		else  
		{
			delete player;
			LOGGER(E_LOG_WARNING) << "unkown level=" << level;
			return -1;
		}
		player->tid = tid;
		player->clevel = level;
		player->playNum = ROBOT_BASE_PLAY_COUNT + rand() % ROBOT_RAND_PLAY_COUNT;

		

		std::string nickname;
		std::string headurl;
		int uid = 0;
		int sex = 0;
		RobotUtil::makeRobotNameSex(Configure::getInstance().m_headLink, nickname, sex, headurl);
		strncpy(player->name, nickname.c_str(), nickname.length());

		Json::Value data;
		data["headUrl"] = headurl;
		data["loseTimes"] = totalnum - winnum;
		data["winTimes"] = winnum;
		data["sex"] = sex;
		data["source"] = E_MSG_SOURCE_ROBOT;
		data["roll"] = roll;
		data["uid"] = player->id;
		data["vip"] = vip;
		data["mostwin"] = double(mostwin);
		strcpy(player->json, data.toStyledString().c_str());

		HallHandler * handler = new HallHandler();
		if (CDLReactor::Instance()->Connect(handler, Configure::getInstance().m_hallIp.c_str(), Configure::getInstance().m_hallPort) < 0)
		{
			delete handler;
			delete player;
			LOGGER(E_LOG_WARNING) << "Connect HallServer error " << Configure::getInstance().m_hallIp << ":" << Configure::getInstance().m_hallPort;
			return -1;
		}
		handler->uid = player->id;
		player->handler = handler;

		PlayerManager::getInstance().addPlayer(player->id, player);
		LOGGER(E_LOG_INFO) << "StartRobot uid=" << player->id << ", tid=" << tid;
		player->login();
	}
	return 0;
}