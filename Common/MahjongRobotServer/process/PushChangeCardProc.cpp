#include "PushChangeCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "Protocol.h"
#include <string>
using namespace std;

REGISTER_PROCESS(GMSERVER_MSG_CHANGED_CARD, PushChangeCardProc)

PushChangeCardProc::PushChangeCardProc()
{
	this->name = "PushChangeCardProc";
}

PushChangeCardProc::~PushChangeCardProc()
{

} 

int PushChangeCardProc::doRequest(CDLSocketHandler* /*client*/, InputPacket* /*pPacket*/, Context* /*pt*/ )
{
	return 0;
}

int PushChangeCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	
	if (PacketBase::game_id != E_SICHUAN_MAHJONG_GAME_ID)
		return EXIT;

	if (retcode < 0 || player == NULL)
		return EXIT;

	int uid = inputPacket->ReadInt();
	if (uid != player->m_Uid)
	{
		LOGGER(E_LOG_ERROR) << "PushChangeCardProc doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}

	player->startChangeCardTimer(uid, 2 + rand() % 3);
	return 0;
}


