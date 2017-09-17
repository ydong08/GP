#include "GameOverProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "RobotRedis.h"
#include <string>
using namespace std;

GameOverProc::GameOverProc()
{
	this->name = "GameOverProc";
}

GameOverProc::~GameOverProc()
{

} 

int GameOverProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int GameOverProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv GameOverProc Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if(retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int winner = inputPacket->ReadInt();
	short levercount = inputPacket->ReadShort();
	for(int i = 0; i < levercount; ++i)
	{
		int leaveid = inputPacket->ReadInt();
		short tabindex = inputPacket->ReadShort();
		string name = inputPacket->ReadString();
		int64_t betcoin = inputPacket->ReadInt64();
	}
	short winnercount = inputPacket->ReadShort();
	if(tid != player->tid)
		return EXIT;
	player->status = tstatus;
	
	player->status = ustatus;

	if(winnercount == 1)
		return EXIT;
	int64_t finalgetCoin = 0;
	for(int i = 0; i < winnercount; i++)
	{
		int anthorid = inputPacket->ReadInt();
		short status = inputPacket->ReadShort();
		short playeridx = inputPacket->ReadShort();
		short finalcardvalue = inputPacket->ReadShort();
		finalgetCoin = inputPacket->ReadInt64();
		int64_t money = inputPacket->ReadInt64();
		if(anthorid == player->id)
		{
			RobotRedis::getInstance()->AddRobotWin(player->clevel, finalgetCoin);
			player->money = money;
		}
		int nwin = inputPacket->ReadInt();
		int nlose = inputPacket->ReadInt();
	}
	
	player->addPlayCount();
	player->reset();
	int64_t winmoney = RobotRedis::getInstance()->getRobotWinCount(player->clevel);

	int switchmoney = 0;
	if (player->clevel == 1)
		switchmoney = Options::instance()->swtichWin1;
	else if(player->clevel == 2)
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

	if(player->clevel == 1 && player->money < 1000)
	{
		player->startLeaveTimer(4);
		return 0;
	}

	if(player->clevel == 2 && player->money < 5000)
	{
		player->startLeaveTimer(4);
		return 0;
	}

	if(player->clevel == 3 && player->money < 500000)
	{
		player->startLeaveTimer(4);
		return 0;
	}


	if(player->getPlayCount() <= player->playNum)
	{
		//if(player->status == STATUS_PLAYER_RSTART)
		//	return 0;
		return CLIENT_MSG_START_GAME;
	}
	player->startLeaveTimer(4);
	return 0;
}


