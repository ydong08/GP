#include "TimeOutProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "RobotDefine.h"
#include <string>
using namespace std;

TimeOutProcess::TimeOutProcess()
{
	this->name = "TimeOutProcess";
}

TimeOutProcess::~TimeOutProcess()
{

} 

int TimeOutProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int TimeOutProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0 || player == NULL)
	{
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "retcode=" << retcode << " retmsg=" << retmsg;
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
		if(player->player_array[i].id == callid)
		{
			player->player_array[i].hascard = 0;
			break;
		}
	}

	if(uid == nextid) {
		player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT + rand() % ROBOT_RAND_BET_TIMEROUT);
		ULOGGER(E_LOG_DEBUG, uid) << "startBetCoinTimer";
	}
	return 0;
}

REGISTER_PROCESS(GMSERVER_BET_TIMEOUT, TimeOutProcess)
