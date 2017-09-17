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
 
	LOGGER(E_LOG_DEBUG) << "[DATA Recv] svid= " << svid;
	 
	GameServer server;
	server.svid = svid;
	int ret = ServerManager::getInstance()->getServer(svid, &server, false);
	if(ret==0)
	{
		LOGGER(E_LOG_DEBUG) << "GetServerInfoProc OK";
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
		LOGGER(E_LOG_DEBUG) << "[DATA Respon] svid = " << server.svid
			<< " ip = " << server.ip << " port = " << server.port
			<< " level = " << server.level << " status = " << server.status
			<< " priority = " << server.priority << " maxusercount = " << server.maxUserCount
			<< " maxtabcount = " << server.maxTabCount << " curr user count = " << server.currUserCount
			<< " curr tabcount = " << server.currTabCount;
		this->send(clientHandler, &ReportPkg);
		
	}
	else
	{
		LOGGER(E_LOG_DEBUG) << "GetServerInfoProc Fault";
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

