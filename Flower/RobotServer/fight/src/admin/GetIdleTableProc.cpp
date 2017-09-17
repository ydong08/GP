#include "GetIdleTableProc.h"
#include "Logger.h"
#include "Configure.h"
#include "Player.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "HallHandler.h"
#include "ProcessManager.h"
#include "RobotDefine.h"
#include "GameCmd.h"
#include <json/json.h>
#include <string>

using namespace std;

GetIdleTableProc::GetIdleTableProc()
{
	this->name = "GetIdleTableProc";
}

GetIdleTableProc::~GetIdleTableProc()
{
} 

int GetIdleTableProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int GetIdleTableProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	int cmd = inputPacket->GetCmdType();
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0)
	{
		LOGGER(E_LOG_ERROR) << "GetIdleTableProc retcode=" << retcode << ", retmsg=" << retmsg;
		return 0;
	}
	int time = inputPacket->ReadInt();
	short count = inputPacket->ReadShort();
	LOGGER(E_LOG_DEBUG) << "Count=" << count;
	for(int i = 0; i < count; i++)
	{
		int tid         = inputPacket->ReadInt();
		short status    = inputPacket->ReadShort();
		short level     = inputPacket->ReadShort();
		short countUser = inputPacket->ReadShort();
		int replayTime  = inputPacket->ReadInt();
		LOGGER(E_LOG_DEBUG) << "tid=" << tid << " status=" << status << " level=" << level;
		if (countUser == 0)
		{
			PlayerManager::productRobot(tid, status, level, countUser);
			PlayerManager::productRobot(tid, status, level, countUser);
			PlayerManager::productRobot(tid, status, level, countUser);
		}
		else if ((countUser > 0 && countUser < LIMIT_FIGHT_ROBOT_NUM) && ((time - replayTime) > ROBOT_ENTER_TABLE_TIME))
		{
			PlayerManager::productRobot(tid, status, level, countUser);
			PlayerManager::productRobot(tid, status, level, countUser);
		}
	}
	return 0;
}

REGISTER_PROCESS(ADMIN_GET_IDLE_TABLE, GetIdleTableProc)