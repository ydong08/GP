#include "LogServerConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "Room.h"
#include "Despatch.h"
#include <string>
#include <stdarg.h>

#define MAX_MSFBUFF_SIZE 1024*1024
static char msgbuff[MAX_MSFBUFF_SIZE]={'\0'};

static LogServerConnect* instance = NULL;

LogServerConnect* LogServerConnect::getInstance()
{
	if(instance==NULL)
	{
		instance = new LogServerConnect();
	}
	return instance;
}

LogServerConnect::LogServerConnect()
{
	heartBeatTimer = new HeartTimer();
	heartBeatTimer->init(this);
}

LogServerConnect::~LogServerConnect()
{
	_LOG_WARN_( "[LogServerConnect::~LogServerConnect()] LogServerConnect was delete\n" );
}

//建立连接
int LogServerConnect::connect(const char* ip, short port)
{
	//启动定时器
	heartBeatTimer->StartTimer(30*1000);

	//建立连接
	strcpy(this->ip, ip);
	this->port = port;
	this->handler = new ConnectHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip, port);
}

//重连接
int LogServerConnect::reconnect()
{
	this->handler = new ConnectHandle(this);
	if( CDLReactor::Instance()->Connect(handler, ip, port) !=0)
	{
		_LOG_WARN_("ReConnect LogServer[%s:%d] Error\n",ip,port);
		return -1;
	}
	else
	{
		_LOG_INFO_("ReConnect LogServer[%s:%d] Ok\n",ip,port);
		return 0;
	}

}

//心跳包
int LogServerConnect::sendHeartBeat()
{
	if(!this->isActive())
		return 0;
	OutputPacket ReportPkg;
	ReportPkg.Begin(CMD_BACKSVR_HEART);
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.End();
	return Send(&ReportPkg);
}

int LogServerConnect::reportLog(int time, int svid, int uid, int opt,const char* format,  ...) 
{
	if(!this->isActive())
		return 0;
	va_list ap; 
	va_start(ap, format); 
	int n = vsnprintf(msgbuff,MAX_MSFBUFF_SIZE,format, ap); 
	va_end(ap); 

	OutputPacket outpacket;
	outpacket.Begin(0x0800);
	outpacket.WriteInt(time); 
	outpacket.WriteInt(svid);
	outpacket.WriteInt(uid);
	outpacket.WriteInt(opt);
	outpacket.WriteString(msgbuff);
	outpacket.End();

	return Send(&outpacket);
}
 
//========================ConnectHandle=============================================
ConnectHandle::ConnectHandle(LogServerConnect* connect)
{
	this->connect = connect;
}

int ConnectHandle::OnConnected()
{
	_LOG_INFO_("Connected LogServer OK. \n");
	this->connect->setActive (true);
	this->connect->sendHeartBeat();
	return 0;
}

// 
int ConnectHandle::OnClose()
{
	_LOG_WARN_( "[%s][%d]:[LogServerConnect::OnClose] LogServerConnect DisConnect\n",__FILE__,__LINE__ );
	this->connect->setActive (false);
	return 0;
}

//  
int ConnectHandle::OnPacketComplete(InputPacket* pPacket)
{
	short cmd = pPacket->GetCmdType() ;
	/*short seqNo = pPacket->GetCmdType();
	_LOG_DEBUG_( "Recv Packet From LogServer,cmd=[0x%04x]\n", cmd );
	_LOG_DEBUG_( "Recv Packet From LogServer,seqNo=[0x%04x]\n", seqNo );
*/
	if(cmd==CMD_BACKSVR_HEART)
	{
		_LOG_DEBUG_( "Recv Packet From LogServer,cmd=[0x%04x]\n", cmd );
		_LOG_DEBUG_( "Recv LogServer HeartBeat Pong\n", cmd );
		return 0;
	}
	Despatch* game_server = Despatch::getInstance();
	 
	return game_server->doBackResponse(0,pPacket);
}
 

//=============================HeartTimer===============================
int HeartTimer::ProcessOnTimerOut()
{
	if(!Configure::getInstance()->isLogReport)
	{
		_LOG_DEBUG_( "==============LogServerConnect HeartTimer1===========\n" );
		StartTimer(30*1000);
		_LOG_DEBUG_( "====================================================\n" );
		return 0;
	}

	_LOG_DEBUG_( "==============LogServerConnect HeartTimer2===========\n" );
	if(connect && connect->isActive())
	{
		if(connect->sendHeartBeat()==0)
			_LOG_DEBUG_( "Send Heart Beat to LogServer OK\n" );
		else
			_LOG_ERROR_( "Send Heart Beat to LogServer ERROR\n" );
	}
	else
	{
		_LOG_WARN_( "LogServerConnect DisConnect, Need ReConnect ...\n" );
		connect->reconnect();
	}
	_LOG_DEBUG_( "====================================================\n" );
	StartTimer(30*1000);
	return 0;
}
 
 
