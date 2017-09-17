#include "GameStartProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

GameStartProc::GameStartProc()
{
	this->name = "GameStartProc";
}

GameStartProc::~GameStartProc()
{

} 

int GameStartProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_START_GAME, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	LOGGER(E_LOG_DEBUG) << "player->id:" << player->id;
	return 0;
}

int GameStartProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	LOGGER(E_LOG_DEBUG) << "retcode=" << retcode << " retmsg=" << retmsg;
	if(retcode == -9 || retcode == -8)
		return 0;
	if(retcode < 0 || player == NULL)
	{
		ULOGGER(E_LOG_INFO, hallHandler->uid) << "[robot exit]";
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int startid = inputPacket->ReadInt();
	short startstatus = inputPacket->ReadShort();
	ULOGGER(E_LOG_DEBUG, uid) << "ustatus=" << ustatus << " tid=" << tid << " tstatus=" << tstatus << " startid=" << startid << " startstatus=" << startstatus;
	player->tstatus = tstatus;
	
	if (tid != player->tid)
	{
		ULOGGER(E_LOG_INFO, uid) << "[robot exit] tid=" << tid << " not equal player->tid=" << player->tid;
		return EXIT;
	}

	if(uid == startid)
	{
		player->status = startstatus;
	}
	else
	{
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player->player_array[i].id == startid)
			{
				player->player_array[i].status = startstatus;
				break;
			}
		}
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_START_GAME, GameStartProc)
