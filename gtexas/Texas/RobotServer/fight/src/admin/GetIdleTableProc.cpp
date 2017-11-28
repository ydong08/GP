#include "GetIdleTableProc.h"
#include "ClientHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "Player.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "HallHandler.h"
#include "SvrConfig.h"
#include "RobotRedis.h"
#include "Util.h"
#include <json/json.h>
#include <string>
#include "Robot/RobotUtil.h"
#include "NamePool.h"
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
		LOGGER(E_LOG_WARNING) << "GetIdleTableProc Error retcode[" <<retcode<< "] retmsg[" <<retmsg.c_str()<< "]";
		//_LOG_ERROR_("GetIdleTableProc Error retcode[%d] retmsg[%s]\n", retcode, retmsg.c_str());
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
		//_LOG_DEBUG_("[Data Recv] status=[%d]\n", status);
		//_LOG_DEBUG_("[Data Recv] level=[%d]\n", level);
		//_LOG_DEBUG_("[Data Recv] countUser=[%d]\n", countUser);
		LOGGER(E_LOG_DEBUG) << "[Data Recv] tid=[" <<tid<< "] [Data Recv] status=[" <<status<< "] [Data Recv] level=["<<level<<"]  [Data Recv] countUser=["<<countUser<<"]";

		if(countUser == 0)
		{
			productRobot(tid, status, level, countUser);
			productRobot(tid, status, level, countUser);
			productRobot(tid, status, level, countUser);
			//productRobot(tid, status, level, countUser);
			//productRobot(tid, status, level, countUser);
		}
		else if((countUser > 0) && ((Time-replayTime) > Configure::instance()->entertime))
		{
			productRobot(tid, status, level, countUser);
			//productRobot(tid, status, level, countUser);
		}
	}
	return 0;
}

int GetIdleTableProc::productRobot(int tid, short status, short level, short countUser)
{
	bool bactive = false;
	//如果这个用户正在玩牌
	if(countUser >= 3)
		return 0;
	//if(level == 2 && countUser == 2)
	//	return 0;
	int64_t winmoney = RobotRedis::getInstance()->getRobotWinCount(level);
	Player* player = new Player();
	if(player)
	{
		player->init();
		player->clevel = level;
		player->playNum = Configure::instance()->baseplayCount + rand()%3;
		int robotuid = PlayerManager::getInstance()->getRobotUID();
		if(robotuid < 0)
		{
			delete player;
			_LOG_ERROR_("robotuid is negative level[%d] robotuid[%d]\n", level, robotuid);
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
			switchmoney = Configure::instance()->swtichWin1;
		else if (level == 2)
			switchmoney = Configure::instance()->swtichWin2;
		else if(level== 3)
			switchmoney = Configure::instance()->swtichWin6;
		if(winmoney > switchmoney)
		{
			int num = rand()%100;
			//if(num < 5)
			player->isKnow = false;
		}
		else
		{
			int num = rand()%100;
			if(num < Configure::instance()->randknow)
				player->isKnow = true;
		}
		int mostwin = 100;
		short viptype = 0;
		if(level == 1)
		{
			player->money = Configure::instance()->baseMoney1 + rand()%Configure::instance()->randMoney1; 
			totalnum += 20 + rand()%380;

			mostwin = 2000 + rand()%30000;
			int rate = 20 + rand()%30;
			winnum = totalnum*rate/100;
			roll += 1;
			viptype = rand()%100 < 10 ? 1 : 0;
		}
		else if (level == 2)
		{
			player->money = Configure::instance()->baseMoney2 + rand()%Configure::instance()->randMoney2; 
			totalnum = 50 + rand()%1500;

			int rate = 20 + rand()%30;
			winnum = totalnum*rate/100;

			mostwin = 20000 + rand()%3000000;
			roll += 150 + rand()%100;
			viptype = rand()%100 < 40 ? 1 : 0;
		}
		else if (level == 3)
		{
			player->money = Configure::instance()->baseMoney3 + rand()%Configure::instance()->randMoney2; 
			totalnum = 50 + rand()%1000;
			winnum = 15 + rand()%400;
			if(totalnum < winnum)
				winnum = totalnum*3/10;
			roll += 250 + rand()%200;
			mostwin = 2000000 + rand()%30000000;
		}
		
		int sex = rand()%2 + 1;
		int randnum = rand()%100;
		char headlink[256] = {0};
        string nickname = NamePool::getInstance()->AllocName();
		strcpy(player->name,nickname.c_str());
		//_LOG_DEBUG_("-------Idle---------uid:%d isKnow:%s winmoney:%d switchmoney:%d \n",player->id, player->isKnow ? "true" : "false", winmoney, switchmoney);
		printf("money =================:%d\n", player->money);
		if(randnum < Configure::instance()->randhead)
		{
			strcpy(headlink, "http://www.baiduc.com/data/icon/default/avatar.jpg");
		}
		else
		{
			strcpy(headlink, "");
		}

		int randuid = Configure::instance()->baseuid + rand()%Configure::instance()->randuid;
		int robotid = randuid+player->id;

		short randredis = rand()%2;
		int num = 0;
		while(true)
		{
			randuid = Configure::instance()->baseuid + rand()%Configure::instance()->randuid;
			robotid = randuid+player->id;
			if(num >10)
				break;
			string nickname, headurl;
			if (RobotUtil::makeRobotNameSex("", nickname, sex, headurl))
				break;
			num++;
		}

		Json::Value data;
		data["picUrl"] = string(headlink);
		data["sum"] = totalnum;
		data["win"] = winnum;
		data["sex"] = sex;
		data["source"] = 30;
		data["lepaper"] = roll;
		data["mostwin"] = double(mostwin);
		data["uid"] = robotid;
		data["vip"] = viptype;
        data["iconid"] = rand() % 6;

		strcpy(player->json,data.toStyledString().c_str());
		HallHandler * handler = new HallHandler();
		if(CDLReactor::Instance()->Connect(handler,Configure::instance()->hall_ip,Configure::instance()->hall_port) < 0)
		{
			delete handler;
			delete player;
			LOGGER(E_LOG_WARNING)<<"Connect BackServer error["<<Configure::instance()->hall_ip<< ":" <<Configure::instance()->hall_port<< "]";
		//	_LOG_ERROR_("Connect BackServer error[%s:%d]\n",Configure::instance()->hall_ip,Configure::instance()->hall_port);
			return -1;
		}
		handler->uid = player->id;
		player->handler = handler;
		PlayerManager::getInstance()->addPlayer(player->id, player);
		
		_LOG_INFO_("StartRobot[%d] tid[%d]\n",player->id, tid);
		player->login();
	}
	return 0;
}
