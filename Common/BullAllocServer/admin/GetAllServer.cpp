#include <string>
#include "GetAllServer.h" 
#include "ServerManager.h"
#include "Logger.h" 
using namespace std;

GetAllServer::GetAllServer()
{
	this->name = "GetAllServerProc";
}
GetAllServer::~GetAllServer()
{

}

int GetAllServer::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	short cmd = pPacket->GetCmdType() ;
	short type = pPacket->ReadShort() ;
	_LOG_DEBUG_( "[DATA Recv] type=[%d]\n", type);
	 
	GameServer*  servers ;
	int count = 0;
	 
	int ret = ServerManager::getInstance()->getAllServer( &servers, &count);
	if(ret==0)
	{
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(0); 
		ReportPkg.WriteString(""); 
		ReportPkg.WriteShort(count); 

		for(int i=0 ; i< count;i++)
		{
			GameServer* server =  &servers[i];

			ReportPkg.WriteShort(server->svid); 
			ReportPkg.WriteString(server->ip);
			ReportPkg.WriteShort(server->port); 
			ReportPkg.WriteShort(server->level); 
			ReportPkg.WriteByte(server->status); 
			ReportPkg.WriteByte(server->priority); 
			ReportPkg.WriteInt(server->maxUserCount); 
			ReportPkg.WriteShort(server->maxTabCount); 
			ReportPkg.WriteInt(server->currUserCount); 
			ReportPkg.WriteShort(server->currTabCount); 
			_LOG_DEBUG_( "[DATA Respon] svid=[%d]\n", server->svid);
			//_LOG_DEBUG_( "[DATA Respon] ip=[%s]\n", server->ip);
			_LOG_DEBUG_( "[DATA Respon] port=[%d]\n", server->port);
			_LOG_DEBUG_( "[DATA Respon] level=[%d]\n", server->level);
			_LOG_DEBUG_( "[DATA Respon] status=[%d]\n",  server->status);
			_LOG_DEBUG_( "[DATA Respon] priority=[%d]\n", server->priority);
			_LOG_DEBUG_( "[DATA Respon] maxUserCount=[%d]\n", server->maxUserCount);
			_LOG_DEBUG_( "[DATA Respon] maxTabCount=[%d]\n", server->maxTabCount);
			_LOG_DEBUG_( "[DATA Respon] currUserCount=[%d]\n", server->currUserCount);
			_LOG_DEBUG_( "[DATA Respon] currTabCount=[%d]\n", server->currTabCount);
			
		}
		
		ReportPkg.End();
		this->send(clientHandler, &ReportPkg);
	}
	else
	{
		_LOG_INFO_("GetAllServer Fault\n");
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(-1); 
		ReportPkg.WriteString("Can't get server\n"); 
		ReportPkg.End();
		this->send(clientHandler, &ReportPkg);
	}
	return 0;
}

int GetAllServer::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt) 
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	return 0;
}

