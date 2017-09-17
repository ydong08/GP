#include <string>
#include "UpdateMemberProc.h" 
#include "ServerManager.h"
#include "Logger.h"
#include "Configure.h"
using namespace std;

UpdateMemberProc::UpdateMemberProc()
{
	this->name = "GMServerMemberProc";
}

UpdateMemberProc::~UpdateMemberProc()
{

}

int UpdateMemberProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt)
{
	_NOTUSED(pt);
	short cmd = pPacket->GetCmdType() ;
	short svid = pPacket->ReadShort();
	short level = pPacket->ReadShort();
	int maxuser = pPacket->ReadInt();
	short maxtab = pPacket->ReadShort();
	int curruser = pPacket->ReadInt();
	short currtab = pPacket->ReadShort();

	int IPhoneUser = pPacket->ReadShort();
	int AndroidUser = pPacket->ReadShort();
	int IPadUser = pPacket->ReadShort();
	int RobotUser = pPacket->ReadShort();
	
	GameServer server;
	server.gameID = Configure::getInstance()->m_nGameID;
	server.svid = svid;
	server.level = level;
	server.maxUserCount = maxuser;
	server.maxTabCount = maxtab;
	server.currUserCount = curruser;
	server.currTabCount = currtab;

	server.IPhoneUser = IPhoneUser;
	server.AndroidUser = AndroidUser;
	server.IPadUser = IPadUser;
	server.RobotUser = RobotUser;

	server.lasttime = time(NULL);
 
	int ret = ServerManager::getInstance()->updateServerMember(svid,   &server);
	
	if(ret==0)
	{
		_LOG_DEBUG_("UpdateMemberProc OK\n");
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(0); 
		ReportPkg.WriteString(""); 
		ReportPkg.End();
		 
		this->send(clientHandler, &ReportPkg,false);
	}
	else
	{
		_LOG_ERROR_("UpdateMemberProc Fault\n");
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(-1); 
		ReportPkg.WriteString("Can't update server member \n"); 
		ReportPkg.End();

		_LOG_DEBUG_( "[DATA Respon] errno=[%d]\n",  0);
		_LOG_DEBUG_( "[DATA Respon] errmsg=[%s]\n", "Can't update server member");

		this->send(clientHandler, &ReportPkg, false);
	}
	return 0;
}

int UpdateMemberProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt) 
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

