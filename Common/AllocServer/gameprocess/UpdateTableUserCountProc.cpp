#include <string>
#include "UpdateTableUserCountProc.h"
#include "ServerManager.h"
#include "Logger.h" 
#include "Configure.h"
#include "AllocManager.h"

using namespace std;

UpdateTableUserCountProc::UpdateTableUserCountProc()
{
	this->name = "UpdateTableUserCountProc";
}

UpdateTableUserCountProc::~UpdateTableUserCountProc()
{

}

int UpdateTableUserCountProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* /*pt*/)
{
	short svid = pPacket->ReadShort();
	int tid = pPacket->ReadInt();
	short level = pPacket->ReadShort();
	short tableUserCount = pPacket->ReadShort();
	int64_t money = pPacket->ReadInt64();
	
	_LOG_DEBUG_("[DATA Recv] svid=[%d]\n", svid);
	_LOG_DEBUG_("[DATA Recv] tid=[%d]\n", tid);
	_LOG_DEBUG_("[DATA Recv] level=[%d]\n", level);
	_LOG_DEBUG_("[DATA Recv] tableUserCount=[%d]\n", tableUserCount);
	_LOG_DEBUG_("[DATA Recv] money=[%ld]\n", money);

	if(tableUserCount < 0 || tableUserCount > Configure::getInstance()->numplayer)
	{
		_LOG_ERROR_("[UpdateTableUserCountProc] tableUserCount[%d] too large or too small\n", tableUserCount);
		return -1;
	}
	
	if (Configure::getInstance()->norobot == 2)
	{
		GameServer* pserver = ServerManager::getInstance()->getServer(svid);
		if(pserver != NULL)
			AllocManager::getInstance()->updateTableUserCount(svid, tid, level, tableUserCount, money);
		else
			_LOG_ERROR_("[UpdateTableUserCountProc] Can't Get server[%d]\n", svid);
	}

	return 0;
}

int UpdateTableUserCountProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt) 
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
