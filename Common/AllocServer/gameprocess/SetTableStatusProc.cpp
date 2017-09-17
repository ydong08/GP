#include <string>
#include "SetTableStatusProc.h"
#include "ServerManager.h"
#include "Logger.h" 
#include "Configure.h"
#include "AllocManager.h"

using namespace std;

SetTableStatusProc::SetTableStatusProc()
{
	this->name = "SetTableStatusProc";
}

SetTableStatusProc::~SetTableStatusProc()
{

}

int SetTableStatusProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* /*pt*/)
{
	//int cmd = pPacket->GetCmdType() ;
	short svid = pPacket->ReadShort();
	int tid = pPacket->ReadInt();
	short level = pPacket->ReadShort();
	short status = pPacket->ReadShort();
	
	_LOG_DEBUG_("[DATA Recv] svid=[%d]\n", svid);
	_LOG_DEBUG_("[DATA Recv] tid=[%d]\n", tid);
	_LOG_DEBUG_("[DATA Recv] level=[%d]\n", level);
	_LOG_DEBUG_("[DATA Recv] status=[%d]\n", status);
	
	if (Configure::getInstance()->norobot == 2)
	{
		GameServer* pserver = ServerManager::getInstance()->getServer(svid);
		if(pserver != NULL)
			AllocManager::getInstance()->setTableStatus(svid, tid, level, status);
		else
			_LOG_ERROR_("[SetTableStatusProc] Can't Get server[%d]\n", svid);
	}
 
	return 0;
}

int SetTableStatusProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt) 
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
