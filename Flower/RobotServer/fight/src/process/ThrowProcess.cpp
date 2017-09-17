#include "ThrowProcess.h"
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

ThrowProcess::ThrowProcess()
{
	this->name = "ThrowProcess";
}

ThrowProcess::~ThrowProcess()
{
} 

int ThrowProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*> (client);
	if (clientHandler == NULL)
		return 0;
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if (player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_THROW_CARD, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << " money=" << player->money << " currmax=" << player->currmax;
	return 0;
}

int ThrowProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*>(clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0 || player == NULL) {
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "retcode=" << retcode << " retmsg=" << retmsg;
		if(retcode == -1)
			return 0;
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	player->currRound = inputPacket->ReadByte();
	int throwid = inputPacket->ReadInt();
	int64_t betcoin = inputPacket->ReadInt64();
	int64_t money = inputPacket->ReadInt64();
	player->currmax = inputPacket->ReadInt64();
	int nextid = inputPacket->ReadInt();
	int64_t sumbetcoin = inputPacket->ReadInt64();
	int64_t selfmoney = inputPacket->ReadInt64();
	player->optype = inputPacket->ReadShort();
	int64_t sumpool = inputPacket->ReadInt64();
	ULOGGER(E_LOG_DEBUG, uid) << "user status:" << ustatus << " tid:" << tid << " table status:" << tstatus << " nextid:" << nextid;
	if (tid != player->tid) {
		ULOGGER(E_LOG_INFO, uid) << "[robot exit] tid=" << tid << " not equal player->tid=" << player->tid;
		return EXIT;
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player->player_array[i].id == throwid) {
			player->player_array[i].hascard = 0;
			break;
		}
	}

	if (uid == nextid)
	{
		player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT + rand() % ROBOT_RAND_BET_TIMEROUT);
		ULOGGER(E_LOG_DEBUG, uid) << "startBetCoinTimer";
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_THROW_CARD, ThrowProcess)