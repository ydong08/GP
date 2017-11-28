#include "GsTransToUserServer.h"
#include "Logger.h"
#include "HallManager.h"
#include "BackConnect.h"

GsTransToUserServer::GsTransToUserServer()
{
	this->name = "GsTransToUserServer";
}

GsTransToUserServer::~GsTransToUserServer()
{

} 

int GsTransToUserServer::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{	
	_NOTUSED(client);
	_NOTUSED(pt);
	int uid = pPacket->ReadInt();
	if(BackConnectManager::getNodes(uid%10 + 1)->send(pPacket)>0)
	{
		_LOG_DEBUG_("Transfer GsTransToUserServer request to Back_UserServer OK\n" );
		return 0;
	}
	else
	{
		_LOG_ERROR_("Transfer GsTransToUserServer request to Back_UserServer Error\n" );
		return -1;
	}
}

int GsTransToUserServer::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

