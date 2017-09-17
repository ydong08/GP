#include "GetPlayInfo.h"
#include "GameCmd.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"

GetPlayInfo::GetPlayInfo()
{
	this->name = "GetPlayInfo";
}

GetPlayInfo::~GetPlayInfo()
{
} 

int GetPlayInfo::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	return 0;
}

REGISTER_PROCESS(ADMIN_GET_PLAY_INFO, GetPlayInfo)
