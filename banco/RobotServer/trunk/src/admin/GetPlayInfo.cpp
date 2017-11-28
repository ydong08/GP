#include "GetPlayInfo.h"
#include "Logger.h"
#include "Configure.h"
#include "Player.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "HallHandler.h"
#include "Util.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <json/json.h>
#include <string>
#include "NamePool.h"

REGISTER_PROCESS(ADMIN_GET_PLAY_INFO, GetPlayInfo);

GetPlayInfo::GetPlayInfo()
{
	this->name = "GetPlayInfo";
}

GetPlayInfo::~GetPlayInfo()
{

} 

int GetPlayInfo::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	_NOTUSED(pPacket);
	_NOTUSED(pt);
	return 0;
}

int GetPlayInfo::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	int cmd = inputPacket->GetCmdType();
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0)
	{
		LOGGER(E_LOG_ERROR) << "GetPlayInfo Error retcode = " << retcode << " retmsg = " << retmsg;
		return 0;
	}
	int tid = inputPacket->ReadInt();
	short status = inputPacket->ReadShort();
	LOGGER(E_LOG_INFO) << "==>[GetPlayInfo] msg = " << cmd << " tid = " << tid << " status = " << status;
	short countPlayer = inputPacket->ReadShort();
	short banklistcount = inputPacket->ReadShort();
	short robotNotInBanklistnum = inputPacket->ReadShort();
	short robotInBanklistnum = inputPacket->ReadShort();
	short winRobotnum = inputPacket->ReadShort();
	LOGGER(E_LOG_DEBUG) << "[GetPlayInfo] countPlayer = " << countPlayer
		<< " banklistcount = " << banklistcount
		<< " robotNotInBanklistnum = " << robotNotInBanklistnum
		<< " winRobotnum = " << winRobotnum
		<< " bankernum = " << PlayerManager::getInstance()->cfg.bankernum
		<< " normalrobotnum = " << PlayerManager::getInstance()->cfg.normalrobotnum
		<< " switchbetter = " << PlayerManager::getInstance()->cfg.switchbetter;
	if(Configure::getInstance()->level == 3)
	{
		if(robotNotInBanklistnum < PlayerManager::getInstance()->cfg.normalrobotnum)
		{
			productBetRobot(tid);
			//productBetRobot(tid);
			//productBetRobot(tid);
		}
	}

	if(Configure::getInstance()->level == 1)
	{
		if(PlayerManager::getInstance()->cfg.switchbanker == 1 && robotInBanklistnum < PlayerManager::getInstance()->cfg.bankernum)
		{
			productBankerRobot(tid);
		}
	}
	return 0;
}

void robotClose(int uid)
{
    PlayerManager::getInstance()->delPlayer(uid, true);
}

int GetPlayInfo::productBankerRobot(int tid)
{
	Player* player = new Player();
	if(player)
	{
        player->init();
		player->tid = tid;
		int playNumCount = Configure::getInstance()->bankerplaybase + rand()%Configure::getInstance()->bankerplayrand;
        player->willPlayCount = playNumCount;

		int totalnum = 0;
        int winnum = 0;
		short roll = 0;

		totalnum = 50 + rand()%1000;
		winnum = (totalnum*(35 + rand()%26))/100;
		roll += 10 + rand()%700;
        short vip = rand()%100 < 90 ? 1 : 0;

        std::string nickname;
        std::string headurl;
        int uid = 0;
        int sex = 0;
		RobotUtil::makeRobotInfo(Configure::getInstance()->m_headLink, sex, headurl, uid);
        nickname = NamePool::getInstance()->AllocName();
		strncpy(player->name, nickname.c_str(), sizeof(player->name));
		player->name[sizeof(player->name)-1] = '\0';

		Json::Value data;
        data["picUrl"] = headurl;
		data["sum"] = totalnum;
        data["win"] = winnum;
		data["lepaper"] = roll;
		data["tips"] = 0;
		data["sex"] = sex;
		data["source"] = 30;
		data["vip"] = vip;
        data["uid"] = uid;
        data["iconid"] = rand() % 6;

		player->id = PlayerManager::getInstance()->getRobotUid();
		if (player->id == -1)
			player->id = uid;
		
// 		int randmoney = Configure::getInstance()->bankerbase;
// 		if (PlayerManager::getInstance()->cfg.carrybankerhigh - PlayerManager::getInstance()->cfg.carrybankerlow > 0)
// 		{
// 			randmoney = PlayerManager::getInstance()->cfg.carrybankerlow + rand()%(PlayerManager::getInstance()->cfg.carrybankerhigh - PlayerManager::getInstance()->cfg.carrybankerlow);
// 		}
//		player->money = randmoney;
		player->money = Util::rand_range(6000000, 10000000);
		strcpy(player->json,data.toStyledString().c_str());
		HallHandler * handler = new HallHandler();
		if(CDLReactor::Instance()->Connect(handler,Configure::getInstance()->hall_ip,Configure::getInstance()->hall_port) < 0)
		{
			delete handler;
			delete player;
			LOGGER(E_LOG_ERROR) << "Connect BackServer error hall ip = " << Configure::getInstance()->hall_ip << " port = " << Configure::getInstance()->hall_port;
			return -1;
		}
        handler->setOnRobotClose(robotClose);
		handler->uid = player->id;
		player->handler = handler;

		PlayerManager::getInstance()->addPlayer(player->id, player);

		LOGGER(E_LOG_INFO) << "StartRobot = " << player->id << " tid = " << tid;
		player->login();
	}
	return 0;
}


int GetPlayInfo::productBetRobot(int tid)
{
	
	Player* player = new Player();
	if(player)
	{
		player->init();
		player->tid = tid;
		int playNumCount = Configure::getInstance()->bankerplaybase + rand() % Configure::getInstance()->bankerplayrand;
		player->willPlayCount = playNumCount;

		int totalnum = 0;
		int winnum = 0;
		short roll = 0;

		totalnum = 50 + rand() % 1000;
		winnum = (totalnum*(35 + rand() % 26)) / 100;
		roll += 10 + rand() % 700;
		short vip = rand() % 100 < 90 ? 1 : 0;

		std::string nickname;
		std::string headurl;
		int uid = 0;
		int sex = 0;
		RobotUtil::makeRobotInfo(Configure::getInstance()->m_headLink, sex, headurl, uid);
		nickname = NamePool::getInstance()->AllocName();
		strncpy(player->name, nickname.c_str(), sizeof(player->name));
		player->name[sizeof(player->name)-1] = '\0';

		Json::Value data;
		data["picUrl"] = headurl;
		data["sum"] = totalnum;
		data["win"] = winnum;
		data["lepaper"] = roll;
		data["tips"] = 0;
		data["sex"] = sex;
		data["source"] = 30;
		data["vip"] = vip;
		data["uid"] = uid;
        data["iconid"] = rand() % 6;

        strncpy(player->json ,data.toStyledString().c_str(), data.toStyledString().size());

		player->id = PlayerManager::getInstance()->getRobotUid();
		if (player->id == -1)
			player->id = uid;
		
		player->money = 10000 + rand() % 90000;

		HallHandler * handler = new HallHandler();
		if(CDLReactor::Instance()->Connect(handler,Configure::getInstance()->hall_ip,Configure::getInstance()->hall_port) < 0)
		{
			delete handler;
			delete player;
			LOGGER(E_LOG_ERROR) << "Connect BackServer error hall ip = " << Configure::getInstance()->hall_ip << " port = " << Configure::getInstance()->hall_port;
			return -1;
		}
		handler->uid = player->id;
		PlayerManager::getInstance()->addPlayer(player->id, player);
		player->handler = handler;
		
		LOGGER(E_LOG_INFO) << "StartRobot = " << player->id << " tid = " << tid;
		player->login();
	}
	return 0;
}

