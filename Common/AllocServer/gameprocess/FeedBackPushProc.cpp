#include "FeedBackPushProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "BackConnect.h"

FeedBackPushProc::FeedBackPushProc()
{
	this->name = "FeedBackPushProc";
}

FeedBackPushProc::~FeedBackPushProc()
{

} 

int FeedBackPushProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	_NOTUSED(client);
	_NOTUSED(pPacket);
	_NOTUSED(pt);
	return 0;
}

int FeedBackPushProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(pt);
	int cmd = inputPacket->GetCmdType();
	int uid = inputPacket->ReadInt();
	short backtype = inputPacket->ReadShort();
	string type = inputPacket->ReadString();
	short hallid = inputPacket->ReadShort();
	_LOG_INFO_("==>[FeedBackPushProc] cmd=[0x%04x] uid=[%d] backtype[%d] hallid[%d]\n", cmd ,uid, backtype, hallid);
	OutputPacket response;
	response.Begin(cmd,uid);
	response.WriteInt(uid);
	response.WriteShort(backtype);
	response.WriteString(type);
	response.End();
	if(HallManager::getInstance()->sendToHall(hallid, &response, false)<0)
		_LOG_ERROR_("Send FeedBackPushProc Info to uid[%d] Error\n", uid);
	else
		_LOG_DEBUG_("Send FeedBackPushProc Info to uid[%d] Success\n", uid);
	
	return 0;
}

