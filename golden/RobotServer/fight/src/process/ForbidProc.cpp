#include "ForbidProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "RobotDefine.h"
#include <string>
using namespace std;

ForbidProc::ForbidProc()
{
	this->name = "ForbidProc";
}

ForbidProc::~ForbidProc()
{

} 

int ForbidProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_FORBIDDEN, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.WriteByte(4);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << " money=" << player->money << " rasecoin=" << player->rasecoin;
	return 0;
}

int ForbidProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0 || player == NULL)
	{
		if(retcode == -4)
		{
			player->comparecard();
			return 0;
		}
		if(retcode == -1)
			return 0;
		ULOGGER(E_LOG_INFO, hallHandler->uid) << "[robot exit]";
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	player->currRound = inputPacket->ReadByte();
	int forbidid = inputPacket->ReadInt();
	short forbidRound = inputPacket->ReadByte();
	int64_t money = inputPacket->ReadInt64();

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player->player_array[i].id == forbidid)
		{
			player->player_array[i].forbidround = forbidRound;
			break;
		}
	}
	ULOGGER(E_LOG_DEBUG, uid) << "ustatus=" << ustatus << " tid=" << tid << " tstatus=" << tstatus;
	if (tid != player->tid)
	{
		ULOGGER(E_LOG_INFO, uid) << "[robot exit] tid=" << tid << " not equal player->tid=" << player->tid;
		return EXIT;
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_FORBIDDEN, ForbidProc)