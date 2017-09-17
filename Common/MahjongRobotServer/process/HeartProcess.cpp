#include "HeartProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(USER_HEART_BEAT, HeartProcess)

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
	requestPacket.Begin(USER_HEART_BEAT, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteInt(player->m_TableId);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int HeartProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	LOGGER(E_LOG_DEBUG) << "";
	return 0;
}

