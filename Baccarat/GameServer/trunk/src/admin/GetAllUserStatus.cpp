#include "GetAllUserStatus.h"
#include "ProtocolServerId.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"

int GetAllUserStatus::doRequest(CDLSocketHandler* client, InputPacket* inputPacket,Context* pt)
{
	_NOTUSED(pt);	
	short cmd = inputPacket->GetCmdType();
	short subcmd = inputPacket->GetSubCmd();
	_LOG_DEBUG_("==>[GetAllUserStatus] [0x%04x] \n", cmd);
	_LOG_DEBUG_("SubCmd=[%d] \n", subcmd);

	OutputPacket response;
	response.Begin(cmd);
	response.WriteShort(0);
	response.WriteString("Ok");
	response.End();
	_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", 0);
	_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", "Ok");
	this->send(client, &response);
	return 0;
}

REGISTER_PROCESS(ADMIN_GET_ALL_PLAYER, GetAllUserStatus)