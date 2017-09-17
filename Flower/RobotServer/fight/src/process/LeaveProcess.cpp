#include "LeaveProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "RobotDefine.h"
#include <string>
#include "Protocol.h"
using namespace std;

LeaveProcess::LeaveProcess()
{
	this->name = "LeaveProcess";
}

LeaveProcess::~LeaveProcess()
{

} 

int LeaveProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_LEAVE, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "Level Table id=" << player->tid;
	return 0;
}

int LeaveProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0 || player == NULL)
	{
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "[robotexit] retcode=" << retcode << " retmsg=" << retmsg;
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	short currRound = inputPacket->ReadByte();
	int leaver = inputPacket->ReadInt();
	player->currmax = inputPacket->ReadInt64();
	int nextid = inputPacket->ReadInt();
	int64_t sumbetcoin = inputPacket->ReadInt64();
	int64_t selfmoney = inputPacket->ReadInt64();
	player->optype = inputPacket->ReadShort();
	int64_t sumpool = inputPacket->ReadInt64();
	if (uid == leaver) {
		ULOGGER(E_LOG_INFO, uid) << "my be leave";
		return EXIT;
	}
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player->player_array[i].id == leaver)
		{
			player->player_array[i].id = 0;
			player->player_array[i].hascard = 0;
			break;
		}
	}
	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT + rand()%ROBOT_RAND_BET_TIMEROUT);
		ULOGGER(E_LOG_INFO, uid) << "startBetCoinTimer";
		return 0;
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player->player_array[i].id != 0 && player->player_array[i].source != E_MSG_SOURCE_ROBOT)
		{
			ULOGGER(E_LOG_INFO, uid) << "player->player_array[i].id=" << player->player_array[i].id;
			return 0;
		}
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_LEAVE, LeaveProcess)