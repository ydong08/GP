#include "ChangeCardResultProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>

REGISTER_PROCESS(GMSERVER_MSG_CHANGE_RESULT, ChangeCardResultProc)

ChangeCardResultProc::ChangeCardResultProc()
{
	this->name = "ChangeCardProc";
}

ChangeCardResultProc::~ChangeCardResultProc()
{
}

int ChangeCardResultProc::doRequest(CDLSocketHandler * client, InputPacket * inputPacket, Context * pt)
{
	return 0;
}

int ChangeCardResultProc::doResponse(CDLSocketHandler * clientHandler, InputPacket * inputPacket, Context * pt)
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();

#ifdef HEBEI
	return EXIT;
#endif

	if (retcode < 0 || player == NULL)
		return EXIT;

	int uid = inputPacket->ReadInt();
	if (uid != player->m_Uid)
	{
		LOGGER(E_LOG_ERROR) << "PushStartPlayProc doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}

	ZeroMemory(player->m_cbCardIndex, sizeof(player->m_cbCardIndex));
	player->m_Card.clear();
	for (int i = 0; i < MAX_COUNT*GAME_PLAYER; i++)
	{
		BYTE card = inputPacket->ReadByte();
		player->m_Card.push_back(card);
	}

	//ÉèÖÃÆË¿Ë
	WORD wMeChairId = player->m_TableIndex;
	BYTE cbCardCount = (wMeChairId == player->banker_pos_) ? MAX_COUNT : (MAX_COUNT - 1);
	player->game_logic_.SwitchToCardIndex((BYTE*)player->m_Card.data(), cbCardCount, player->m_cbCardIndex[wMeChairId]);
	BYTE bIndex = 1;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == wMeChairId) continue;
		cbCardCount = (i == player->banker_pos_) ? MAX_COUNT : (MAX_COUNT - 1);
		player->game_logic_.SwitchToCardIndex((BYTE*)&player->m_Card[MAX_COUNT*bIndex++], cbCardCount, player->m_cbCardIndex[i]);
	}

	return 0;
}
