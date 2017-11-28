#include "PhpReChargeProc.h"
#include "Logger.h"
#include "HallManager.h"

PhpReChargeProc::PhpReChargeProc()
{
	this->name = "PhpReChargeProc";
}

PhpReChargeProc::~PhpReChargeProc()
{

} 

int PhpReChargeProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	_NOTUSED(client);
	_NOTUSED(pPacket);
	_NOTUSED(pt);
	return 0;
}

int PhpReChargeProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(pt);
	int cmd = inputPacket->GetCmdType();
	int uid = inputPacket->ReadInt();
	string type = inputPacket->ReadString();
	short hallid = inputPacket->ReadShort();
	_LOG_INFO_("==>[PhpReChargeProc] cmd=[0x%04x] uid=[%d] type[%s] hallid[%d]\n", cmd ,uid, type.c_str(), hallid);
	OutputPacket response;
	response.Begin(cmd,uid);
	response.WriteInt(uid);
	response.WriteString(type);
	response.End();
	if(HallManager::getInstance()->sendToHall(hallid, &response, false)<0)
		_LOG_ERROR_("Send PhpReChargeProc Info to uid[%d] Error\n", uid);
	else
		_LOG_DEBUG_("Send PhpReChargeProc Info to uid[%d] Success\n", uid);
	
	return 0;
}

