#include "PushDefineLackProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "Protocol.h"
#include <string>
using namespace std;

REGISTER_PROCESS(GMSERVER_MSG_DEFINED_LACK, PushDefineLackProc)

PushDefineLackProc::PushDefineLackProc()
{
	this->name = "PushDefineLackProc";
}

PushDefineLackProc::~PushDefineLackProc()
{

} 

int PushDefineLackProc::doRequest(CDLSocketHandler* /*client*/, InputPacket* /*pPacket*/, Context* /*pt*/ )
{
	return 0;
}

int PushDefineLackProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
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
		LOGGER(E_LOG_ERROR) << "PushDefineLackProc doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}

	player->startDefineLackTimer(uid, 2 + rand() % 3);
	return 0;
}


