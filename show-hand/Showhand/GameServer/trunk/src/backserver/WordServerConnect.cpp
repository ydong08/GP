#include "WordServerConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "Room.h"
#include "Despatch.h"
#include <string>
#include "CDLReactor.h" 

static WordServerConnect* instance = NULL;

WordServerConnect* WordServerConnect::getInstance()
{
	if(instance==NULL)
	{
		instance = new WordServerConnect();
	}
	return instance;
}

WordServerConnect::WordServerConnect()
{
	heartBeatTimer = new WordHeartBeatTimer();
	heartBeatTimer->init(this);
}

WordServerConnect::~WordServerConnect()
{
	_LOG_ERROR_( "[WordServerConnect::~WordServerConnect] WordServerConnect was delete\n" );
}

//建立连接
int WordServerConnect::connect(const char* ip, short port)
{
	//启动定时器
	heartBeatTimer->StartTimer(30*1000);

	//建立连接
	strcpy(this->ip, ip);
	this->port = port;
	this->handler = new WordSocketHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip,port);
}

//重连接
int WordServerConnect::reconnect()
{
	this->handler = new WordSocketHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip,port);
}

//心跳包
int WordServerConnect::sendHeartBeat()
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(CMD_BACKSVR_HEART);
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.End();
	return Send(&ReportPkg);
}
 
//========================WordSocketHandle=============================================
WordSocketHandle::WordSocketHandle(WordServerConnect* server)
{
	this->server = server;
}

int WordSocketHandle::OnConnected()
{
	_LOG_INFO_("Connected WordServer.\n");
	this->server->setActive (true);
	this->server->sendHeartBeat();
	return 0;
}

// 
int WordSocketHandle::OnClose()
{
	_LOG_ERROR_( "[WordServerConnect::OnClose] WordServerConnect DisConnect\n" );
	this->server->setActive (false);
	return 0;
}

//  
int WordSocketHandle::OnPacketComplete(InputPacket* pPacket)
{
	short cmd = pPacket->GetCmdType() ;
	/*short seqNo = pPacket->GetCmdType();
	_LOG_DEBUG_( "Recv Packet From Word  Server,cmd=[0x%04x]\n", cmd );
	_LOG_DEBUG_( "Recv Packet From Word  Server,seqNo=[0x%04x]\n", seqNo );
*/
	if(cmd==CMD_BACKSVR_HEART)
	{
		_LOG_DEBUG_( "Recv Packet From WordServer,cmd=[0x%04x]\n", cmd );
		_LOG_DEBUG_( "Recv WordServer HeartBeat Pong\n", cmd );
		return 0;
	}
	Despatch* game_server = Despatch::getInstance();
	 
	return game_server->doBackResponse(0,pPacket);
}
 

//=============================WordHeartBeatTimer===============================
 
int WordHeartBeatTimer::ProcessOnTimerOut()
{
	_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "=========WordServerConnect WordHeartBeatTimer===========\n" );
	if(backsrv && backsrv->isActive())
	{
		if(backsrv->sendHeartBeat()==0)
			_LOG_DEBUG_( "Send Heart Beat to Word Server OK\n" );
		else
		{
			_LOG_ERROR_( "Send Heart Beat to Word Server ERROR\n" );
			backsrv->setActive(false);
			backsrv->reconnect();
		}
	}
	else
	{
		_LOG_DEBUG_( "WordSvrConnect DisConnect, Need ReConnect ...\n" );
		backsrv->reconnect();
	}
	_LOG_DEBUG_( "=======================================\n" );
	StartTimer(30*1000);
	return 0;
}
 
