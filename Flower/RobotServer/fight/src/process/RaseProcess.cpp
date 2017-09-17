#include "RaseProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "RobotDefine.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

RaseProcess::RaseProcess()
{
	this->name = "RaseProcess";
}

RaseProcess::~RaseProcess()
{

} 

int RaseProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_BET_RASE, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.WriteInt64(player->rasecoin);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << " money=" << player->money << " rasecoin=" << player->rasecoin;
	return 0;
}

int RaseProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*>(clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	if (player == NULL) {
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "[robotexit] player not found";
		return EXIT;
	}
	int    retcode = inputPacket->ReadShort();
	string retmsg  = inputPacket->ReadString();
	if(retcode == -4) {
		player->comparecard();
		return 0;
	} else if(retcode == -1) {
		return 0;
	} else if (retcode < 0) {
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "[robotexit] retcode=" << retcode << " retmsg=" << retmsg;
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
	player->sumbetcoin = inputPacket->ReadInt64();
	int64_t selfmoney = inputPacket->ReadInt64();
	player->optype = inputPacket->ReadShort();
	int64_t sumpool = inputPacket->ReadInt64();
	ULOGGER(E_LOG_DEBUG, uid) << "user status=" << ustatus << " tid=" << tid << " table status=" << tstatus << " nextid=" << nextid;

	if (tid != player->tid) {
		ULOGGER(E_LOG_INFO, uid) << "[robot exit] tid=" << tid << " not equal player->tid=" << player->tid;
		return EXIT;
	}

	if(uid == nextid)
	{
		if(player->isRaseMax())
			player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT);
		else
			player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT + rand() % ROBOT_RAND_BET_TIMEROUT);
		ULOGGER(E_LOG_DEBUG, uid) << "startBetCoinTimer";
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_BET_RASE, RaseProcess)