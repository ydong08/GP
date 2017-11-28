#include "AllinProcess.h"
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

AllinProcess::AllinProcess()
{
	this->name = "AllinProcess";
}

AllinProcess::~AllinProcess()
{

} 

int AllinProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_ALL_IN, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << " money=" << player->money << " currmax=" << player->currmax;
	return 0;
}

int AllinProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
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
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	player->currRound = inputPacket->ReadByte();
	int callid = inputPacket->ReadInt();
	int64_t callcoin = inputPacket->ReadInt64();
	int64_t betcoin = inputPacket->ReadInt64();
	int64_t money = inputPacket->ReadInt64();
	player->currmax = inputPacket->ReadInt64();
	int nextid = inputPacket->ReadInt();
	int64_t sumbetcoin = inputPacket->ReadInt64();
	int64_t selfmoney = inputPacket->ReadInt64();
	player->optype = inputPacket->ReadShort();
	int64_t sumpool = inputPacket->ReadInt64();
	ULOGGER(E_LOG_DEBUG, uid) << "status=" << ustatus << " tid=" << tid << " status=" << tstatus << " nextid=" << nextid;
	if(tid != player->tid)
		return EXIT;

	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT + rand()%ROBOT_RAND_BET_TIMEROUT);
		ULOGGER(E_LOG_DEBUG, uid) << "startBetCoinTimer";
	}

	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_ALL_IN, AllinProcess)

