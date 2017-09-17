#include "HallHeartBeatProc.h"
#include "Logger.h"
#include "HallManager.h"

HallHeartBeatProc::HallHeartBeatProc()
{
	this->name = "HallHeartBeatProc";
}

HallHeartBeatProc::~HallHeartBeatProc()
{

} 

int HallHeartBeatProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType() ;

	int hallid =  pPacket->ReadInt();
	string msg = pPacket->ReadString();
	_LOG_DEBUG_("[Recv Data] cmd=[0x%04x] id=[%d]\n", cmd, hallid );
	_LOG_DEBUG_("[Recv Data] msg=[%s]\n",msg.c_str() );
	
	return 0;
}

int HallHeartBeatProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
