#include "DefineLackProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>

REGISTER_PROCESS(CLIENT_MSG_DEFINED_LACK, DefineLackProc)

DefineLackProc::DefineLackProc()
{
	this->name = "DefineLackProc";
}

DefineLackProc::~DefineLackProc()
{
}

int DefineLackProc::doRequest(CDLSocketHandler * client, InputPacket * inputPacket, Context * pt)
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if (player == NULL)
		return 0;

	uint8_t			defineColor;

	defineColor = player->GetDefaultLackColor();

	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_DEFINED_LACK, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteInt(player->m_TableId);
	requestPacket.WriteByte(defineColor);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int DefineLackProc::doResponse(CDLSocketHandler * clientHandler, InputPacket * inputPacket, Context * pt)
{
	return 0;
}
