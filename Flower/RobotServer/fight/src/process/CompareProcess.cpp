#include "CompareProcess.h"
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

CompareProcess::CompareProcess()
{
	this->name = "CompareProcess";
}

CompareProcess::~CompareProcess()
{

} 

int CompareProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*>(client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_COMPARE_CARD, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.WriteInt(player->compareUid);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << " money=" << player->money << " currmax=" << player->currmax;
	return 0;
}

int CompareProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*>(clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0 || player == NULL)
	{
		ULOGGER(E_LOG_INFO, hallHandler->uid) << "[robot exit]";
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	player->currRound = inputPacket->ReadByte();
	int64_t compareCoin = inputPacket->ReadInt64();
	short compareRet = inputPacket->ReadByte();
	int compareid = inputPacket->ReadInt();
	int64_t comparebetcoin = inputPacket->ReadInt64();
	int64_t comparemoney = inputPacket->ReadInt64();
	int opponentid = inputPacket->ReadInt();
	player->currmax = inputPacket->ReadInt64();
	int nextid = inputPacket->ReadInt();
	int64_t sumbetcoin = inputPacket->ReadInt64();
	int64_t selfmoney = inputPacket->ReadInt64();
	player->optype = inputPacket->ReadShort();
	int64_t sumpool = inputPacket->ReadInt64();
	ULOGGER(E_LOG_DEBUG, uid) << "ustatus=" << ustatus << " tid=" << tid << " tstatus=" << tstatus << " nextid=" << nextid;
	if (tid != player->tid)
	{
		ULOGGER(E_LOG_INFO, uid) << "[robot exit] tid=" << tid << " not equal player->tid=" << player->tid;
		return EXIT;
	}

	if(compareRet == 0)
	{
		if(compareid == uid)
			player->hascard = 0;
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player->player_array[i].id == compareid)
			{
				player->player_array[i].hascard = 0;
				break;
			}
		}
	}

	if(compareRet == 1)
	{
		if(opponentid == uid)
			player->hascard = 0;
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player->player_array[i].id == opponentid)
			{
				player->player_array[i].hascard = 0;
				break;
			}
		}
	}

	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT * 2 + rand()%ROBOT_RAND_BET_TIMEROUT);
		ULOGGER(E_LOG_INFO, uid) << "startBetCoinTimer.";
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_COMPARE_CARD, CompareProcess)