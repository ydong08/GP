#include "PushStartPlayProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
#include "Protocol.h"
using namespace std;

REGISTER_PROCESS(GMSERVER_MSG_PLAYING, PushStartPlayProc)

PushStartPlayProc::PushStartPlayProc()
{
	this->name = "PushStartPlayProc";
}

PushStartPlayProc::~PushStartPlayProc()
{

} 

int PushStartPlayProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int PushStartPlayProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
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
		LOGGER(E_LOG_ERROR) << "PushStartPlayProc doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}

	player->banker_pos_ = inputPacket->ReadInt();
	player->current_user_pos_ = inputPacket->ReadInt();
	player->action_ = inputPacket->ReadByte();
	
	if ((player->action_ != WIK_NULL) || player->current_user_pos_ == player->m_TableIndex)
	{
		player->startOutCardTimer(uid, 2 + rand() % 3);
	}
	return 0;
}


