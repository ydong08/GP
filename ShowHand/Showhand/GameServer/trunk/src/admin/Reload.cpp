#include <string>

#include "Reload.h"
//#include "ScoreSys.h"
#include "../GameCmd.h"
#include "../model/Room.h"
#include "ErrorMsg.h"
#include "Logger.h"
#include "ErrorMessage.h"
using namespace std;

int Reload::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();
	_LOG_INFO_("==>[Reload] [0x%04x] \n", cmd);
	Configure::getInstance()->read_conf("../conf/showhand.ini");
	ErrorMsg::getInstance()->initFromBuf(g_errorMsg);
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
