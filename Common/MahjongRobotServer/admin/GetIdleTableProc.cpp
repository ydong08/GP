#include "GetIdleTableProc.h"
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

REGISTER_PROCESS(ADMIN_GET_IDLE_TABLE, GetIdleTableProc)

GetIdleTableProc::GetIdleTableProc()
{
	this->name = "GetIdleTableProc";
}

GetIdleTableProc::~GetIdleTableProc()
{

} 

int GetIdleTableProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	_NOTUSED(clientHandler);
	_NOTUSED(pPacket);
	_NOTUSED(pt);
	return 0;
}

int GetIdleTableProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	int cmd = inputPacket->GetCmdType();
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0)
	{
		LOGGER(E_LOG_ERROR) << "GetIdleTableProc Error retcode[" << retcode << "] retmsg[" << retmsg << "]";
		return 0;
	}
	int Time = inputPacket->ReadInt();
	short Count = inputPacket->ReadShort();
	LOGGER(E_LOG_INFO) << "==>[GetIdleTableProc] [" << cmd << "] Count=[" << Count << "]";
	for(int i = 0; i < Count; i++)
	{
		int tid = inputPacket->ReadInt();
		short status = inputPacket->ReadShort();
		short level = inputPacket->ReadShort();
		short countUser = inputPacket->ReadShort();
		int replayTime = inputPacket->ReadInt();
		LOGGER(E_LOG_DEBUG) << "[Data Recv] tid = " << tid << " status = " << status << " level =" << level
							<< " countUser:" << countUser;
		if (countUser == 0)
		{
			PlayerManager::productRobot(tid, status, level, countUser);
			PlayerManager::productRobot(tid, status, level, countUser);
		}
		else if ((countUser > 0) /* &&  ((Time - replayTime) > Configure::getInstance().enter_time)*/)
		{
			PlayerManager::productRobot(tid, status, level, countUser);
		}
	}
	return 0;
}