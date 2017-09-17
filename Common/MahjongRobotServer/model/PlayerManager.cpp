#include <stdlib.h>
#include <string.h>
#include <map>
#include <algorithm>

#include "GameCmd.h"
#include "PlayerManager.h"
#include "Logger.h"
#include "HallHandler.h"
#include "Util.h"
#include "Configure.h"
#include "json/json.h"
#include "NamePool.h"
#include "Protocol.h"
#include "RobotUtil.h"
#include "Packet.h"

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
	m_nStartID = RobotUtil::getRobotStartUid(PacketBase::game_id, Configure::getInstance().level, Configure::getInstance().robot_server_index);
	LOGGER(E_LOG_INFO) << "Robot Start uid=" << m_nStartID;
	
	m_allocInfos.resize(IDNUM);
	for (int i = 0; i < IDNUM; ++i)	
	{
		m_frees.push_back(i);
		m_allocInfos[i] = false;
	}
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

void  PlayerManager::delPlayer(int uid, bool isclose)
{
	Player* player = getPlayer(uid);
	if(player)
	{
		ULOGGER(E_LOG_INFO, player->m_Uid) << "exit uid";
		delRobotUid(player->m_Uid);
		if(!isclose)
		{
			if(player->handler)
				delete player->handler;
		}

		NamePool::getInstance()->FreeName(player->name.c_str());
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
	std::list<int>::iterator		it;
	int								id = -1;

	it = m_frees.begin();
	if (it != m_frees.end())
	{
		id = *it;
		m_allocInfos[id] = true;
		id += m_nStartID;
		m_frees.pop_front();
	}
	
	return id;
}

int PlayerManager::delRobotUid(int robotuid)
{
	int		id = robotuid - m_nStartID;
	
	ASSERT(id >= 0);
	if (m_allocInfos[id])
	{
		m_frees.push_front(id);
		m_allocInfos[id] = false;
	}

	return 0;
}

int PlayerManager::productRobot(int tid, short status, short level, short countUser)
{
	if (level > E_MASTER_LEVEL || level < E_PRIMARY_LEVEL)
	{
		LOGGER(E_LOG_ERROR) << "level is not invalid:" << level;
		return -1;
	}
	Player* player = new Player();
	if (player)
	{
		player->init();
		player->clevel = level;
		player->playNum = Configure::getInstance().play_count + rand() % 3;
		int robotuid = PlayerManager::getInstance()->getRobotUID();
		if (robotuid < 0)
		{
			delete player;
			ULOGGER(E_LOG_ERROR, robotuid) << "robotuid is negative level = " << level;
			return -1;
		}
		player->m_Uid = robotuid;
		player->m_TableId = tid;
		int totalnum = 0;
		int winnum = 0;
		int roll = 0;
		int mostwin = 0;
		player->money = Configure::getInstance().base_money[level - 1];
		if (level == E_PRIMARY_LEVEL)
		{
			player->money += rand() % 500;
			totalnum += 5 + rand() % 6;
			winnum += 1 + rand() % 3;
			//roll += 1;
			mostwin = 10 * (1 + rand() % 300);
		}
		else if (level == E_MIDDLE_LEVEL)
		{
			player->money += rand() % 1500;
			totalnum = 50 + rand() % 800;
			winnum = 10 + rand() % 300;
			if (totalnum < winnum)
				winnum = totalnum * 3 / 10;

			roll += 150 + rand() % 100;
			mostwin = 100 * (1 + rand() % 300);
		}
		else if (level == E_ADVANCED_LEVEL)
		{
			player->money += rand() % 163000;
			totalnum = 50 + rand() % 1000;
			winnum = 15 + rand() % 400;
			if (totalnum < winnum)
				winnum = totalnum * 3 / 10;
			roll += 250 + rand() % 200;
			mostwin = 300 * (1 + rand() % 300);
		}
		else if (level == E_MASTER_LEVEL)
		{
			player->money += rand() % 200000;
			totalnum = 50 + rand() % 10000;
			winnum = 15 + rand() % 800;
			if (totalnum < winnum)
				winnum = totalnum * 3 / 10;
			roll += 250 + rand() % 200;
			mostwin = 500 * (1 + rand() % 300);
		}

		std::string nickname;
		std::string headurl;
		int uid = 0;
		int sex = 0;
		RobotUtil::makeRobotInfo(Configure::getInstance().head_url, sex, headurl, uid);
        player->name = nickname = NamePool::getInstance()->AllocName();

		roll = rand() % 1000;

		Json::Value data;
		data["picUrl"] = string(headurl);
		data["sum"] = totalnum;
		data["win"] = winnum;
		data["sex"] = sex;
		data["source"] = 30;
		data["lepaper"] = roll;
		int robotid = player->m_Uid;
		data["uid"] = robotid;
		data["mostwin"] = double(mostwin);

		short vip = 0;
		int randnum = rand() % 100;
		if (randnum < Configure::getInstance().rand_vip[0])
		{
			randnum = rand() % 100;
			if (randnum < Configure::getInstance().rand_vip[1])
				vip = 1;
			else if (randnum >= Configure::getInstance().rand_vip[1] && randnum < Configure::getInstance().rand_vip[2])
				vip = 2;
			else if (randnum >= Configure::getInstance().rand_vip[2] && randnum < Configure::getInstance().rand_vip[3])
				vip = 10;
			else if (randnum >= Configure::getInstance().rand_vip[3])
				vip = 100;
		}

		data["vip"] = vip;
		data["level"] = 1 + rand() % 10;
		player->json = data.toStyledString();

		HallHandler * handler = new HallHandler();
		if (CDLReactor::Instance()->Connect(handler, Configure::getInstance().hall_ip.c_str(), Configure::getInstance().hall_port) < 0)
		{
			delete handler;
			delete player;
			LOGGER(E_LOG_ERROR) << "Connect BackServer error[" << Configure::getInstance().hall_ip << ":" << Configure::getInstance().hall_port << "]";
			return -1;
		}
		handler->uid = player->m_Uid;
		player->handler = handler;
		PlayerManager::getInstance()->addPlayer(player->m_Uid, player);

		ULOGGER(E_LOG_INFO, player->m_Uid) << "start robot, tid = " << (tid&0x0F) << " name = " << player->name;
		player->login();
	}
	return 0;
}

