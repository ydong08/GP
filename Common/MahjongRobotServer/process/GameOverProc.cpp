#include "GameOverProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(GMSERVER_MSG_GAMEOVER, GameOverProc)

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
	//return EXIT;
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	
	if (player)
		player->stopOutCardTimer();

	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if (retcode < 0 || player == NULL)
		return EXIT;
	
	player->addPlayCount();
	if(player->getPlayCount() >= player->playNum)
	{
		player->startLeaveTimer(3);
		return 0;
	}
	else
	{
		return CLIENT_MSG_START_GAME;
	}
}


