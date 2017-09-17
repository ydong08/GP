#include "TrumptConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "Despatch.h"
#include <string>
#include "CDLReactor.h" 

static TrumptConnect* instance = NULL;

TrumptConnect* TrumptConnect::getInstance()
{
	if(instance==NULL)
	{
		instance = new TrumptConnect();
	}
	return instance;
}

TrumptConnect::TrumptConnect()
{
	heartBeatTimer = new TrumptHeartBeatTimer();
	heartBeatTimer->init(this);
}

TrumptConnect::~TrumptConnect()
{
	_LOG_ERROR_( "[TrumptConnect::~TrumptConnect] TrumptConnect was delete\n" );
}

static void registAlloc(int id,CDLSocketHandler* handler)
{
    OutputPacket OutPkg;
    OutPkg.Begin(ALLOC_REGIST);
	OutPkg.WriteInt(Configure::instance()->m_nServerId);
    OutPkg.End();
    if(handler && handler->Send(OutPkg.packet_buf(),OutPkg.packet_size())>0 )
        _LOG_INFO_("Trumpet Regist\n");
}

//建立连接
int TrumptConnect::connect(const char* ip, short port)
{
	//启动定时器
	heartBeatTimer->StartTimer(30*1000);

	//建立连接
	//strcpy(this->ip, ip);
	this->ip = ip;
	this->port = port;
	this->handler = new TrumptSocketHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip,port);
}

//重连接
int TrumptConnect::reconnect()
{
	this->handler = new TrumptSocketHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip.c_str(),port);
}

//心跳包
int TrumptConnect::sendHeartBeat()
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(CMD_BACKSVR_HEART);
	ReportPkg.WriteShort(Configure::instance()->m_nServerId);
	ReportPkg.End();
	return Send(&ReportPkg);
}
 
//========================TrumptSocketHandle=============================================
TrumptSocketHandle::TrumptSocketHandle(TrumptConnect* server)
{
	this->server = server;
}

int TrumptSocketHandle::OnConnected()
{
	_LOG_INFO_("Connected TrumptServer.\n");
	this->server->setActive (true);
	this->server->sendHeartBeat();
	registAlloc(1,this);
	return 0;
}

// 
int TrumptSocketHandle::OnClose()
{
	_LOG_ERROR_( "[TrumptConnect::OnClose] TrumptConnect DisConnect\n" );
	this->server->setActive (false);
	return 0;
}

//  
int TrumptSocketHandle::OnPacketComplete(InputPacket* pPacket)
{
	short cmd = pPacket->GetCmdType() ;
	/*short seqNo = pPacket->GetCmdType();
	_LOG_DEBUG_( "Recv Packet From Trumpt  Server,cmd=[0x%04x]\n", cmd );
	_LOG_DEBUG_( "Recv Packet From Trumpt  Server,seqNo=[0x%04x]\n", seqNo );
*/
	if(cmd==CMD_BACKSVR_HEART)
	{
		_LOG_DEBUG_( "Recv Packet From TrumptServer,cmd=[0x%04x]\n", cmd );
		_LOG_DEBUG_( "Recv TrumptServer HeartBeat Pong\n", cmd );
		return 0;
	}
	Despatch* game_server = Despatch::getInstance();
	 
	return game_server->doBackResponse(0,pPacket);
}
 

//=============================TrumptHeartBeatTimer===============================
 
int TrumptHeartBeatTimer::ProcessOnTimerOut()
{
	_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "=========TrumptConnect TrumptHeartBeatTimer===========\n" );
	if(backsrv && backsrv->isActive())
	{
		if(backsrv->sendHeartBeat()==0)
			_LOG_DEBUG_( "Send Heart Beat to Trumpt Server OK\n" );
		else
		{
			_LOG_ERROR_( "Send Heart Beat to Trumpt Server ERROR\n" );
			backsrv->setActive(false);
			backsrv->reconnect();
		}
	}
	else
	{
		_LOG_DEBUG_( "TrumptSvrConnect DisConnect, Need ReConnect ...\n" );
		backsrv->reconnect();
	}
	_LOG_DEBUG_( "=======================================\n" );
	StartTimer(30*1000);
	return 0;
}
 
