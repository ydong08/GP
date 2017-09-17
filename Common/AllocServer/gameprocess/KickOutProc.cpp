#include "KickOutProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "BackConnect.h"

KickOutProc::KickOutProc()
{
	this->name = "KickOutProc";
}

KickOutProc::~KickOutProc()
{

} 

int KickOutProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	_NOTUSED(client);
	_NOTUSED(pPacket);
	_NOTUSED(pt);
	return 0;
}

int KickOutProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(pt);
	int cmd = inputPacket->GetCmdType();
	int uid = inputPacket->ReadInt();
	short hallid = inputPacket->ReadShort();
	_LOG_INFO_("==>[KickOutProc] cmd=[0x%04x] uid=[%d] hallid[%d]\n", cmd ,uid, hallid);
	OutputPacket response;
	response.Begin(cmd,uid);
	response.WriteInt(uid);
	response.End();
	if(HallManager::getInstance()->sendToHall(hallid, &response, false)<0)
		_LOG_ERROR_("Send KickOut Info to uid[%d] Error\n", uid);
	else
		_LOG_DEBUG_("Send KickOut Info to uid[%d] Success\n", uid);
	
	return 0;
}

