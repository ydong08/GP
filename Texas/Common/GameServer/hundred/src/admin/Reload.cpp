#include <string>

#include "Reload.h"
//#include "ScoreSys.h"
#include "GameCmd.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "ProtocolServerId.h"

REGISTER_PROCESS(ADMIN_CMD_RELOAD, Reload)
using namespace std;

int Reload::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();
	_LOG_INFO_("==>[Reload] [0x%04x] \n", cmd);
	OutputPacket OutPkg;
	OutPkg.Begin(cmd);
	OutPkg.WriteShort(0); 
	OutPkg.WriteString("Ok"); 
	OutPkg.End();
	this->send(clientHandler, &OutPkg);
	return 0;
}

int Reload::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	return 0;	
}  

