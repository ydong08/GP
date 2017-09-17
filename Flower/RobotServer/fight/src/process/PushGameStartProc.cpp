#include "PushGameStartProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "RobotDefine.h"
#include <string>
using namespace std;

PushGameStartProc::PushGameStartProc()
{
	this->name = "PushGameStartProc";
}

PushGameStartProc::~PushGameStartProc()
{

} 

int PushGameStartProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int PushGameStartProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if (retcode < 0 || player == NULL)
	{
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "[robotexit] retcode=" << retcode << " retmsg=" << retmsg;
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int ante = inputPacket->ReadInt();
	int tax = inputPacket->ReadInt();
	short dealeruid = inputPacket->ReadInt();
	int firstuid = inputPacket->ReadInt();
	int64_t sumpool = inputPacket->ReadInt64();
	if (tid != player->tid) {
		ULOGGER(E_LOG_WARNING, uid) << "[robotexit] this robot tid:" << player->tid << " != tid:" << tid;
		return EXIT;
	}
	player->tstatus = tstatus;
	player->status  = ustatus;
	
	player->hascard = 1;
	player->ante = ante;
	player->currmax = ante;

	BYTE count = inputPacket->ReadByte();
	player->countnum = count;
	for(int i =0; i < count; ++i)
	{
		int otherid = inputPacket->ReadInt();
		int64_t tempmoney = inputPacket->ReadInt64();
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(player->player_array[j].id == otherid)
			{
				player->player_array[j].hascard = 1;
				break;
			}
		}
	}
	player->optype = inputPacket->ReadShort();

	for(int i = 0; i < 5; i++)
	{
		player->rasecoinarray[i] = inputPacket->ReadInt();
	}

	if(player->id == firstuid) 
	{
		player->startBetCoinTimer(uid, ROBOT_BASE_BET_TIMEROUT + ROBOT_RAND_BET_TIMEROUT);
		ULOGGER(E_LOG_INFO, uid) << "startBetCoinTimer";
	}
	return 0;
}

REGISTER_PROCESS(GMSERVER_GAME_START, PushGameStartProc);
