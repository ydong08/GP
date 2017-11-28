#include "HeartProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(USER_HEART_BEAT, HeartProcess)
using namespace std;

HeartProcess::HeartProcess()
{
	this->name = "HeartProcess";
}

HeartProcess::~HeartProcess()
{

} 

int HeartProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(USER_HEART_BEAT, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int HeartProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	LOGGER(E_LOG_DEBUG)<<"doResponse HeartProcess";
	//_LOG_DEBUG_("doResponse HeartProcess\n");
	return 0;
}

