#include "GetFreePlayerProc.h"
#include "Logger.h"
#include "Configure.h"
#include "Player.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "RobotDefine.h"
#include "Protocol.h"
#include <json/json.h>
#include <string>
using namespace std;

GetFreePlayerProc::GetFreePlayerProc()
{
	this->name = "GetFreePlayerProc";
}

GetFreePlayerProc::~GetFreePlayerProc()
{

} 

int GetFreePlayerProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int GetFreePlayerProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	int cmd = inputPacket->GetCmdType();
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0)
	{
		LOGGER(E_LOG_ERROR) << "GetFreePlayerProc Error retcode=" << retcode << " retmsg=" << retmsg;
		return 0;
	}

	int	  time = inputPacket->ReadInt();
	short count = inputPacket->ReadShort();
	LOGGER(E_LOG_INFO) << "count =" << count;

	for (int i = 0; i < count; i++)
	{
		int     uid = inputPacket->ReadInt();
		int     tid = inputPacket->ReadInt();
		short   level = inputPacket->ReadShort();
		int64_t carrycoin = inputPacket->ReadInt64();
		short   source = inputPacket->ReadShort();
		short   status = inputPacket->ReadShort();
		int     enterTime = inputPacket->ReadInt();

		ULOGGER(E_LOG_INFO, uid) << "tid=" << tid << " level=" << level << " carrycoin=" << carrycoin << " source=" << source << " status=" << status
			<< " enterTime=" << enterTime << " has coming time=" << time - enterTime;

		if (((time - enterTime) > ROBOT_ENTER_TABLE_TIME) && (source != E_MSG_SOURCE_ROBOT))
		{
			PlayerManager::productRobot(tid, status, level, 0);
		}
	}
	return 0;
}


REGISTER_PROCESS(ADMIN_GET_FREE_PLAYER, GetFreePlayerProc)