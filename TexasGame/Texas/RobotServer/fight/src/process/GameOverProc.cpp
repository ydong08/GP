#include "GameOverProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
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
	short countplayer = inputPacket->ReadByte();
	if(tid != player->tid)
		return EXIT;
	
	player->status = ustatus;

	
	short level = player->clevel;
// 	short loadtype = 1;
// 	if(level <= LEVEL1)
// 		loadtype = 1;
// 	else if (level > LEVEL1 && level <= LEVEL2)
// 		loadtype = 2;
// 	else if (level > LEVEL2 && level <= LEVEL3)
// 		loadtype = 3;
// 	else if (level >= LEVEL6)
// 		loadtype = 6;

	int64_t finalgetCoin = 0;
	for(int i = 0; i < countplayer; i++)
	{
		int anthorid = inputPacket->ReadInt();
		short status = inputPacket->ReadShort();
		short playeridx = inputPacket->ReadByte();
		short cardtype = inputPacket->ReadByte();
		finalgetCoin = inputPacket->ReadInt64();
		int64_t money = inputPacket->ReadInt64();
		int64_t carrymoney = inputPacket->ReadInt64();
		if(anthorid == player->id)
		{
			RobotRedis::getInstance()->AddRobotWin(level, finalgetCoin);
		//	_LOG_DEBUG_("_____________uid:%d finalgetCoin:%d carrymoney:%ld \n", anthorid, finalgetCoin, carrymoney);
			player->money = money;
			player->carrycoin = carrymoney;
		}
		int nwin = inputPacket->ReadInt();
		int nlose = inputPacket->ReadInt();
		short poolnum = inputPacket->ReadByte();
		for(int j = 0; j < poolnum;++j)
		{
			inputPacket->ReadByte();
			inputPacket->ReadInt64();
		}
	}
	

	//_LOG_DEBUG_("------GameOver----------uid:%d \n",player->id);

	LOGGER(E_LOG_DEBUG)<< "------GameOver----------uid:"<<player->id;


	if(level == 1 && player->carrycoin < 200)
	{
		player->startLeaveTimer(4);
		return 0;
	}

	if(level == 2 && player->carrycoin < 4000)
	{
		player->startLeaveTimer(4);
		return 0;
	}

	if(level == 6 && player->money < 50000)
	{
		player->startLeaveTimer(4);
		return 0;
	}


	player->addPlayCount();
	player->reset();

	if(player->getPlayCount() <= player->playNum)
	{
		player->startStartGameTimer(2);
		return 0;
	}
	player->startLeaveTimer(4);
	return 0;
}


