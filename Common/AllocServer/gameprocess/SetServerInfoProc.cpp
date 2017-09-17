#include <string>
#include "SetServerInfoProc.h"
#include "ServerManager.h"
#include "HallManager.h"
#include "Logger.h"
#include "Configure.h"
#include "AllocManager.h"
using namespace std;

SetServerInfoProc::SetServerInfoProc()
{
	this->name = "SetServerInfoProc";
}

SetServerInfoProc::~SetServerInfoProc()
{

}
/*注：此处和AllocManager里面的所有的type都是类似于ServerManager里的level*/

int SetServerInfoProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt)
{
	_NOTUSED(pt);
	int cmd = pPacket->GetCmdType() ;
	short svid = pPacket->ReadShort();
	string ip = pPacket->ReadString();
	short port = pPacket->ReadShort();
	short level = pPacket->ReadShort();
	short status = pPacket->ReadByte();
	short priority = pPacket->ReadByte();
	int maxuser = pPacket->ReadInt();
	short maxtab = pPacket->ReadShort();
	int curruser = pPacket->ReadInt();
	short currtab = pPacket->ReadShort();
	
	_LOG_DEBUG_( "[DATA Recv] svid=[%d]\n", svid);
	_LOG_DEBUG_( "[DATA Recv] ip=[%s]\n", ip.c_str());
	_LOG_DEBUG_( "[DATA Recv] port=[%d]\n", port);
	_LOG_DEBUG_( "[DATA Recv] level=[%d]\n", level);
	_LOG_DEBUG_( "[DATA Recv] status=[%d]\n", status);
	_LOG_DEBUG_( "[DATA Recv] priority=[%d]\n", priority);
	_LOG_DEBUG_( "[DATA Recv] maxuser=[%d]\n", maxuser);
	_LOG_DEBUG_( "[DATA Recv] maxtab=[%d]\n", maxtab);
	_LOG_DEBUG_( "[DATA Recv] curruser=[%d]\n", curruser);
	_LOG_DEBUG_( "[DATA Recv] currtab=[%d]\n", currtab);

	GameServer* pserver = ServerManager::getInstance()->getServer(svid);
	if(pserver == NULL)
	{
		pserver = new GameServer();
		pserver->gameID = Configure::getInstance()->m_nGameID;
		pserver->svid = svid;
		pserver->ip = ip;
		pserver->port = port;
		pserver->level = level;
		pserver->status = status;
		pserver->priority = priority;
		pserver->maxUserCount = maxuser;
		pserver->maxTabCount = maxtab;
		pserver->currUserCount = curruser;
		//pserver->currTabCount = currtab;
		pserver->lasttime = time(NULL);
		int ret = ServerManager::getInstance()->addServer(svid, pserver);
		if(ret == 0)
		{
			_LOG_INFO_("SetServerInfoProc OK\n");
			ServerManager::getInstance()->addGameServerHandler(svid, clientHandler);
		}
		else
			_LOG_ERROR_("SetServerInfoProc Error Svid[%d]\n", svid);

		OutputPacket ReportPkg;
		ReportPkg.Begin(ALLOC_TRAN_GAMEINFO);
		ReportPkg.WriteShort(1);
		ReportPkg.WriteShort(svid);
		ReportPkg.WriteString(ip.c_str());
		ReportPkg.WriteShort(port);
		ReportPkg.End();
		HallManager::getInstance()->sendAllHall(&ReportPkg, false);
	}


	if(pserver != NULL && Configure::getInstance()->norobot == 2)
	{
		for(int i =0 ; i < currtab; ++i)
		{
			int tid = pPacket->ReadInt();
			int tableType = pPacket->ReadShort();
			short tableUserCount = pPacket->ReadShort();
			int64_t money = pPacket->ReadInt64();
			if((tid>>16) == svid)
			{
				if(AllocManager::getInstance()->addTable(tid, svid, tableType, tableUserCount, money)==0)
				{
					_LOG_DEBUG_("[SetServerInfoProc] Add Table[%d][%d][%d] Sucess\n", tid, tid >> 16, tid & 0x0000FFFF);
					
				}
				else
				{
					_LOG_DEBUG_("[SetServerInfoProc] Add Table[%d][%d][%d] Error\n", tid, tid >> 16, tid & 0x0000FFFF);
				}
			}
			else
				_LOG_ERROR_("[SetServerInfoProc] report tid's svid[%d] not equal report svid[%d]\n", tid >> 16, svid);
		}
	}
	OutputPacket ReportPkg;
	ReportPkg.Begin(cmd);
	ReportPkg.WriteShort(0); 
	ReportPkg.WriteString(""); 
	ReportPkg.End();				 
	this->send(clientHandler, &ReportPkg, false);
 
	return 0;
}

int SetServerInfoProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt) 
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
