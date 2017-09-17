

#include "ChangeCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>

REGISTER_PROCESS(CLIENT_MSG_CHANGED_CARD, ChangeCardProc)

ChangeCardProc::ChangeCardProc()
{
	this->name = "ChangeCardProc";
}

ChangeCardProc::~ChangeCardProc()
{
}

int ChangeCardProc::doRequest(CDLSocketHandler * client, InputPacket * inputPacket, Context * pt)
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if (player == NULL)
		return 0;

	uint8_t			changeCards[3];

	player->GetDefaultChangeCard(changeCards);
	ULOGGER(E_LOG_INFO, player->m_Uid) << "Default Change card = " << TOHEX(changeCards[0]) << ", " << TOHEX(changeCards[1]) << ", " << TOHEX(changeCards[2]);

	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_CHANGED_CARD, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteInt(player->m_TableId);
	for (int i = 0; i < 3; i++)
		requestPacket.WriteByte(changeCards[i]);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int ChangeCardProc::doResponse(CDLSocketHandler * clientHandler, InputPacket * inputPacket, Context * pt)
{
	return 0;
}
