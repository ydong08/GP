#include "MySqlConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "Room.h"
#include "Despatch.h"
#include <string>
#include "CDLReactor.h" 

static MySqlConnect* instance = NULL;

MySqlConnect* MySqlConnect::getInstance()
{
	if(instance==NULL)
	{
		instance = new MySqlConnect();
	}
	return instance;
}

MySqlConnect::MySqlConnect()
{
	heartBeatTimer = new HeartBeatTimer();
	heartBeatTimer->init(this);
}

MySqlConnect::~MySqlConnect()
{
	_LOG_ERROR_( "[MySqlConnect::~MySqlConnect] MySqlConnect was delete\n" );
}

//建立连接
int MySqlConnect::connect(const char* ip, short port)
{
	//启动定时器
	heartBeatTimer->StartTimer(30*1000);

	//建立连接
	strcpy(this->ip, ip);
	this->port = port;
	this->handler = new MySqlHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip,port);
}

//重连接
int MySqlConnect::reconnect()
{
	this->handler = new MySqlHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip,port);
}

//心跳包
int MySqlConnect::sendHeartBeat()
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(CMD_BACKSVR_HEART);
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.End();
	return Send(&ReportPkg);
}
 
//========================MySqlHandle=============================================
MySqlHandle::MySqlHandle(MySqlConnect* server)
{
	this->server = server;
}

int MySqlHandle::OnConnected()
{
	_LOG_INFO_("Connected mysql server. \n");
	this->server->setActive (true);
	this->server->sendHeartBeat();
	return 0;
}

// 
int MySqlHandle::OnClose()
{
	_LOG_ERROR_( "[MySqlConnect::OnClose] MySqlConnect DisConnect\n" );
	this->server->setActive (false);
	return 0;
}

//  
int MySqlHandle::OnPacketComplete(InputPacket* pPacket)
{
	short cmd = pPacket->GetCmdType() ;
	/*short seqNo = pPacket->GetCmdType();
	_LOG_DEBUG_( "Recv Packet From MySql Server,cmd=[0x%04x]\n", cmd );
	_LOG_DEBUG_( "Recv Packet From MySql Server,seqNo=[0x%04x]\n", seqNo );
*/
	if(cmd==CMD_BACKSVR_HEART)
	{
		_LOG_DEBUG_( "Recv Packet From MySql Server,cmd=[0x%04x]\n", cmd );
		_LOG_DEBUG_( "Recv MySql Server HeartBeat Pong\n", cmd );
		return 0;
	}
	Despatch* game_server = Despatch::getInstance();
	 
	return game_server->doBackResponse(0,pPacket);
}
 

//=============================HeartBeatTimer===============================

int HeartBeatTimer::ProcessOnTimerOut()
{
	_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "=========MySqlConnect HeartBeatTimer===========\n" );
	if(backsrv && backsrv->isActive())
	{
		if(backsrv->sendHeartBeat()==0)
			_LOG_DEBUG_( "Send Heart Beat to MySql Server OK\n" );
		else
			_LOG_ERROR_( "Send Heart Beat to MySql Server ERROR\n" );
	}
	else
	{
		_LOG_DEBUG_( "MysqlSvrConnect DisConnect, Need ReConnect ...\n" );
		backsrv->reconnect();
	}
	_LOG_DEBUG_( "=======================================\n" );
	StartTimer(30*1000);
	return 0;
}
 
