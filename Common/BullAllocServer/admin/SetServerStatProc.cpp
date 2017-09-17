 #include <string>
 #include "SetServerStatProc.h" 
 #include "ServerManager.h"
 #include "Logger.h" 
 using namespace std;

SetServerStatProc::SetServerStatProc()
{
	this->name = "SetServerStatusProc";
}

SetServerStatProc::~SetServerStatProc()
{

}

int SetServerStatProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	short cmd = pPacket->GetCmdType() ;
	short subcmd = pPacket->GetSubCmd() ;
	short svid = pPacket->ReadShort();
	char status = pPacket->ReadByte();
 
	_LOG_DEBUG_( "[DATA Recv] svid=[%d]\n", svid);
	_LOG_DEBUG_( "[DATA Recv] status=[%d]\n", status);
 
 
	int ret = 0;
	if(subcmd==0)
		ret = ServerManager::getInstance()->setServerStatus(svid,  status );
	else
		ret = ServerManager::getInstance()->setServerPriority(svid, status );
	
	if(ret==0)
	{
		_LOG_INFO_("SetServerStatProc OK\n");
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(0); 
		ReportPkg.WriteString(""); 
		ReportPkg.End();

		_LOG_DEBUG_( "[DATA Respon] errno=[%d]\n",  0);
		_LOG_DEBUG_( "[DATA Respon] errmsg=[%s]\n", "");
		 
		this->send(clientHandler, &ReportPkg);
		//更新服务器状态，同时设置游戏服务器的Status
		CDLSocketHandler* gamesvr   = ServerManager::getInstance()->getGameServerHandler(svid);
		if(gamesvr)
		{
			OutputPacket packet;
			packet.Begin(cmd);
			packet.SetSubCmd(subcmd);
			packet.WriteShort(svid); 
			packet.WriteByte(status); 
			packet.End();
			this->send(gamesvr, &packet, false);
		}
	}
	else
	{
		_LOG_ERROR_("SetServerStatProc Fault\n");
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(-1); 
		ReportPkg.WriteString("Can't Set Server Stat \n"); 
		ReportPkg.End();

		_LOG_DEBUG_( "[DATA Respon] errno=[%d]\n",  0);
		_LOG_DEBUG_( "[DATA Respon] errmsg=[%s]\n", "Can't update server member");

		this->send(clientHandler, &ReportPkg);
	}


	return 0;
}

int SetServerStatProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt) 
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	return 0;
}
