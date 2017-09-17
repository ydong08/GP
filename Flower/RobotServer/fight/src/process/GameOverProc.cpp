#include "GameOverProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"

#include "Configure.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "RobotDefine.h"
#include <string>
using namespace std;

GameOverProc::GameOverProc()
{
	this->name = "GameOverProc";
}

GameOverProc::~GameOverProc()
{

} 

int GameOverProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int GameOverProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
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
	if (tid != player->tid)
	{
		ULOGGER(E_LOG_INFO, uid) << "[robot exit] tid=" << tid << " not equal player->tid=" << player->tid;
		return EXIT;
	}
	player->tstatus = tstatus;
	
	player->status = ustatus;
	short winnercount = inputPacket->ReadByte();
	int64_t finalgetCoin = 0;
	for(int i = 0; i < winnercount; ++i)
	{
		int anthorid = inputPacket->ReadInt();
		short antorstatus = inputPacket->ReadShort();
		short index = inputPacket->ReadByte();
		short cardstatus = inputPacket->ReadByte();
		LOGGER(E_LOG_DEBUG) << "anthorid:" << anthorid << " card1:" << TOHEX(inputPacket->ReadByte()) <<
			" card2:" << TOHEX(inputPacket->ReadByte()) << " card3:" << TOHEX(inputPacket->ReadByte()) <<
			" cardtype:" << TOHEX(inputPacket->ReadByte());
		finalgetCoin = inputPacket->ReadInt64();
		int64_t newmoney = inputPacket->ReadInt64();
		int win = inputPacket->ReadInt();
		int lose = inputPacket->ReadInt();
		int roll = inputPacket->ReadInt();
		//if (player->id == anthorid)
			//RobotRedis::getInstance().addRobotWin(player->clevel, finalgetCoin);
	}

	player->playCount++;

	LOGGER(E_LOG_DEBUG) << "player id:" << player->id << " playCount:" << player->playCount << " playNum:" << player->playNum;
	if(player->playCount <= player->playNum)
	{
		player->reset();
		player->startGameStartTimer(player->id, ROBOT_RESTART_TIMEOUT);
		return 0;
	}
	player->startLeaveTimer(ROBOT_LEVEL_ROOM_TIMEOUT);
	return 0;
}

REGISTER_PROCESS(GMSERVER_MSG_GAMEOVER, GameOverProc)
