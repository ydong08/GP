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
	LOGGER(E_LOG_DEBUG) << "[DATA Recv] type = " << type;
	 
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
			LOGGER(E_LOG_DEBUG) << "[DATA Respon] svid = " << server->svid
				<< " ip = " << server->ip << " port = " << server->port
				<< " level = " << server->level << " status = " << server->status
				<< " priority = " << server->priority << " maxusercount = " << server->maxUserCount
				<< " maxtabcount = " << server->maxTabCount << " curr user count = " << server->currUserCount
				<< " curr tabcount = " << server->currTabCount;
			
		}
		
		ReportPkg.End();
		this->send(clientHandler, &ReportPkg);
	}
	else
	{
		LOGGER(E_LOG_INFO) << "GetAllServer Fault";
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

