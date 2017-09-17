#include "TrumptProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "TrumptConnect.h"
#include <string>
using namespace std;

TrumptProc::TrumptProc()
{
	this->name = "TrumptProc";
}

TrumptProc::~TrumptProc()
{

} 

int TrumptProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{	
	if(TrumptConnect::getInstance()->Send(pPacket)>0)
	{
		_LOG_DEBUG_("Transfer TrumptProc request to Back_UserServer OK\n" );
		return 1;
	}
	else
	{
		_LOG_ERROR_("Transfer TrumptProc request to Back_UserServer Error\n" );
		return -1;
	}
	return 0;
}

int TrumptProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(pt);
	int cmd = inputPacket->GetCmdType();
	short type = inputPacket->ReadShort();
	if(type == 0)
		return 0;
	string msg = inputPacket->ReadString();
	short pid = inputPacket->ReadShort();
	short gameid = inputPacket->ReadShort();
	LOGGER(E_LOG_INFO) << "cmd = " << TOHEX(cmd) << " type = " << 
		type << " pid = " << pid << " gameid = " << gameid
		<< " msg = " << msg;
	OutputPacket response;
	response.Begin(cmd);
	response.WriteShort(type);
	response.WriteString(msg);
	response.WriteShort(pid);
	response.WriteShort(gameid);
	response.End();
	if(HallManager::getInstance()->sendAllHall(&response, false)<0)
		_LOG_ERROR_("Send TrumptProc Info to type[%d] Error\n", type);
	else
		_LOG_DEBUG_("Send TrumptProc Info to type[%d] Success\n", type);
	return 0;
}

