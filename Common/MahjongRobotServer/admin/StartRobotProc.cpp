#include "StartRobotProc.h"
#include "ClientHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "Player.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "HallHandler.h"
#include "RobotSvrConfig.h"
#include "Protocol.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <json/json.h>
#include <string>
using namespace std;

REGISTER_PROCESS(START_ROBOT, StartRobotProc)

StartRobotProc::StartRobotProc()
{
	this->name = "StartRobotProc";
}

StartRobotProc::~StartRobotProc()
{

} 

int StartRobotProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	int cmd = pPacket->GetCmdType();
	int tid = pPacket->ReadInt();
	short Count = pPacket->ReadShort();
	short type = pPacket->ReadShort();
	LOGGER(E_LOG_INFO) << "==>[StartRobotProc] cmd = " << cmd << " tid = " << tid << " type = " << type;
	PlayerManager::productRobot(tid, 0, type, Count);
	return 0;
}

int StartRobotProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	return 0;
}

