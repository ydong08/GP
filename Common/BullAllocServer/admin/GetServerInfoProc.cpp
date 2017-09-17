 #include <string>
 #include "GetServerInfoProc.h" 
 #include "ServerManager.h"
  #include "Logger.h" 
 using namespace std;

GetServerInfoProc::GetServerInfoProc()
{
	this->name = "GetServerInfoProc";
}
GetServerInfoProc::~GetServerInfoProc()
{

}


int GetServerInfoProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	short cmd = pPacket->GetCmdType() ;
	short svid = pPacket->ReadShort();
 
	_LOG_DEBUG_( "[DATA Recv] svid=[%d]\n", svid);
	 
	GameServer server;
	server.svid = svid;
	int ret = ServerManager::getInstance()->getServer(svid, &server, false);
	if(ret==0)
	{
		_LOG_INFO_("GetServerInfoProc OK\n");
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(0); 
		ReportPkg.WriteString(""); 
		ReportPkg.WriteShort(server.svid);
		ReportPkg.WriteString(server.ip);
		ReportPkg.WriteShort(server.port);
		ReportPkg.WriteShort(server.level);
		ReportPkg.WriteByte(server.status);
		ReportPkg.WriteByte(server.priority);
		ReportPkg.WriteInt(server.maxUserCount);
		ReportPkg.WriteShort(server.maxTabCount);
		ReportPkg.WriteInt(server.currUserCount);
		ReportPkg.WriteShort(server.currTabCount);
		ReportPkg.End();
		_LOG_DEBUG_( "[DATA Respon] svid=[%d]\n", server.svid);
		//_LOG_DEBUG_( "[DATA Respon] ip=[%s]\n", server.ip);
		_LOG_DEBUG_( "[DATA Respon] port=[%d]\n", server.port);
		_LOG_DEBUG_( "[DATA Respon] level=[%d]\n", server.level);
		_LOG_DEBUG_( "[DATA Respon] status=[%d]\n",  server.status);
		_LOG_DEBUG_( "[DATA Respon] priority=[%d]\n", server.priority);
		_LOG_DEBUG_( "[DATA Respon] maxUserCount=[%d]\n", server.maxUserCount);
		_LOG_DEBUG_( "[DATA Respon] maxTabCount=[%d]\n", server.maxTabCount);
		_LOG_DEBUG_( "[DATA Respon] currUserCount=[%d]\n", server.currUserCount);
		_LOG_DEBUG_( "[DATA Respon] currTabCount=[%d]\n", server.currTabCount);
		this->send(clientHandler, &ReportPkg);
		
	}
	else
	{
		_LOG_INFO_("GetServerInfoProc Fault\n");
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(-1); 
		ReportPkg.WriteString("Can't get server info \n"); 
		ReportPkg.End();
		this->send(clientHandler, &ReportPkg);
	}
	return 0;
}

int GetServerInfoProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt) 
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	return 0;
}

