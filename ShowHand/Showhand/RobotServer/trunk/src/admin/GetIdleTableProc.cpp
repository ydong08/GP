#include "GetIdleTableProc.h"
#include "ClientHandler.h"
#include "Logger.h"
#include "Options.h"
#include "Player.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "HallHandler.h"
#include "SvrConfig.h"
#include "RobotRedis.h"
#include "VerifyRedis1.h"
#include "VerifyRedis2.h"
#include "Util.h"
#include <json/json.h>
#include <string>
using namespace std;

GetIdleTableProc::GetIdleTableProc()
{
	this->name = "GetIdleTableProc";
}

GetIdleTableProc::~GetIdleTableProc()
{

} 

int GetIdleTableProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	_NOTUSED(clientHandler);
	_NOTUSED(pPacket);
	_NOTUSED(pt);
	return 0;
}

int GetIdleTableProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	int cmd = inputPacket->GetCmdType();
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0)
	{
		//_LOG_ERROR_("GetIdleTableProc Error retcode[%d] retmsg[%s]\n", retcode, retmsg.c_str());
		LOGGER(E_LOG_WARNING) << "GetIdleTableProc Error retcode["<<retcode<<"] retmsg["<<retmsg.c_str()<<"]";
		return 0;
	}
	int Time = inputPacket->ReadInt();
	short Count = inputPacket->ReadShort();
//	_LOG_INFO_("==>[GetIdleTableProc] [0x%04x] Count=[%d]\n", cmd, Count);
	for(int i = 0; i < Count; i++)
	{
		int tid = inputPacket->ReadInt();
		short status = inputPacket->ReadShort();
		short level = inputPacket->ReadShort();
		short countUser = inputPacket->ReadShort();
		int replayTime = inputPacket->ReadInt();
	//	_LOG_DEBUG_("[Data Recv] tid=[%d]\n", tid);
	//	_LOG_DEBUG_("[Data Recv] status=[%d]\n", status);
	//	_LOG_DEBUG_("[Data Recv] level=[%d]\n", level);
		LOGGER(E_LOG_DEBUG) << "[Data Recv] tid=["<<tid<<"] [Data Recv] status=["<<status<<"]  [Data Recv] level=["<<level<<"]";
		if(countUser == 0)
		{
			productRobot(tid, status, level, countUser);
			productRobot(tid, status, level, countUser);
		}
		else if((countUser > 0) && ((Time-replayTime) > Options::instance()->entertime))
			productRobot(tid, status, level, countUser);
	}
	return 0;
}

int GetIdleTableProc::productRobot(int tid, short status, short level, short countUser)
{
	bool bactive = false;
	//如果这个用户正在玩牌
	if(countUser > 2)
		return 0;
	int64_t winmoney = RobotRedis::getInstance()->getRobotWinCount(level);
	Player* player = new Player();
	if(player)
	{
		player->init();
		player->clevel = level;
		player->playNum = Options::instance()->baseplayCount + rand()%3;
		int robotuid = PlayerManager::getInstance()->getRobotUID();
		strcpy(player->name,SvrConfig::getInstance()->getRandNameNode()->name);
		if(robotuid < 0)
		{
			delete player;
			LOGGER(E_LOG_WARNING) << "robotuid is negative level["<<level<<"] robotuid["<<robotuid<<"]";
			//_LOG_ERROR_("robotuid is negative level[%d] robotuid[%d]\n", level, robotuid);
			return -1;
		}
		player->id = robotuid;
		player->tid = tid;
		if (bactive)
		{
			player->bactive = true;
		}
		int totalnum = 0;
		int winnum = 0;
		int roll = 0;
		int switchmoney = 0;
		if (level == 1)
			switchmoney = Options::instance()->swtichWin1;
		else if (level == 2)
			switchmoney = Options::instance()->swtichWin2;
		else 
			switchmoney = Options::instance()->swtichWin3;
		if(winmoney > switchmoney)
		{
			int num = rand()%100;
			//if(num < 5)
			player->isKnow = false;
		}
		else
		{
			int num = rand()%100;
			if(num < 60)
				player->isKnow = true;
		}
		if(level == 1)
		{
			player->money = Options::instance()->baseMoney1 + rand()%Options::instance()->randMoney1; 
			totalnum += 20 + rand()%80;

			int rate = 30 + rand()%10;
			winnum = totalnum*rate/100;
			//roll += 1;
		}
		else if (level == 2)
		{
			player->money = Options::instance()->baseMoney2 + rand()%Options::instance()->randMoney2; 
			totalnum = 50 + rand()%900;

			int rate = 30 + rand()%10;
			winnum = totalnum*rate/100;

			roll += 150 + rand()%100;
		}
		else if (level == 3)
		{
			player->money = Options::instance()->baseMoney3 + rand()%Options::instance()->randMoney2; 
			totalnum = 50 + rand()%1000;
			winnum = 15 + rand()%400;
			if(totalnum < winnum)
				winnum = totalnum*3/10;
			roll += 250 + rand()%200;
		}

		int randnum = rand()%100;
		std::string nickname;
		std::string headurl;
		int sex = 0;
		Util::makeRobotNameSex(Options::instance()->m_headLink, nickname, sex, headurl);
		strncpy(player->name, nickname.c_str(), nickname.length());

		Json::Value data;
		data["picUrl"] = headurl;
		data["totalNum"] = totalnum;
		data["winNum"] = winnum;
		data["sex"] = sex;
		data["source"] = 30;
		data["lePapers"] = roll;
		data["uid"] = player->id;
        data["iconid"] = rand() % 6;

		strcpy(player->json,data.toStyledString().c_str());
		HallHandler * handler = new HallHandler();
		if(CDLReactor::Instance()->Connect(handler,Options::instance()->hall_ip,Options::instance()->hall_port) < 0)
		{
			delete handler;
			delete player;
			LOGGER(E_LOG_WARNING) << "Connect BackServer error["<<Options::instance()->hall_ip<<":"<<Options::instance()->hall_port<<"]";
		//	_LOG_ERROR_("Connect BackServer error[%s:%d]\n",Options::instance()->hall_ip,Options::instance()->hall_port);
			return -1;
		}
		handler->uid = player->id;
		player->handler = handler;
		PlayerManager::getInstance()->addPlayer(player->id, player);
		
	//	_LOG_INFO_("StartRobot[%d] tid[%d]\n",player->id, tid);
		LOGGER(E_LOG_WARNING) << "StartRobot["<<player->id<<"] tid["<<tid<<"]";
		player->login();
	}
	return 0;
}

