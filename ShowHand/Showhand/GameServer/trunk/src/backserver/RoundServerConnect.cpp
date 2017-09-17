#include "RoundServerConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "Room.h"
#include "Despatch.h"
#include <string>
#include "CDLReactor.h" 

static RoundServerConnect* instance = NULL;

RoundServerConnect* RoundServerConnect::getInstance()
{
	if(instance==NULL)
	{
		instance = new RoundServerConnect();
	}
	return instance;
}

RoundServerConnect::RoundServerConnect()
{
	heartBeatTimer = new RoundHeartBeatTimer();
	heartBeatTimer->init(this);
}

RoundServerConnect::~RoundServerConnect()
{
	_LOG_ERROR_( "[RoundServerConnect::~RoundServerConnect] RoundServerConnect was delete\n" );
}

//建立连接
int RoundServerConnect::connect(const char* ip, short port)
{
	//启动定时器
	heartBeatTimer->StartTimer(30*1000);

	//建立连接
	strcpy(this->ip, ip);
	this->port = port;
	this->handler = new RoundSocketHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip,port);
}

//重连接
int RoundServerConnect::reconnect()
{
	this->handler = new RoundSocketHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip,port);
}

//心跳包
int RoundServerConnect::sendHeartBeat()
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(CMD_BACKSVR_HEART);
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.End();
	return Send(&ReportPkg);
}
 
//========================RoundSocketHandle=============================================
RoundSocketHandle::RoundSocketHandle(RoundServerConnect* server)
{
	this->server = server;
}

int RoundSocketHandle::OnConnected()
{
	_LOG_INFO_("Connected RoundServer.\n");
	this->server->setActive (true);
	this->server->sendHeartBeat();
	return 0;
}

// 
int RoundSocketHandle::OnClose()
{
	_LOG_ERROR_( "[RoundServerConnect::OnClose] RoundServerConnect DisConnect\n" );
	this->server->setActive (false);
	return 0;
}

//  
int RoundSocketHandle::OnPacketComplete(InputPacket* pPacket)
{
	short cmd = pPacket->GetCmdType() ;
	/*short seqNo = pPacket->GetCmdType();
	_LOG_DEBUG_( "Recv Packet From Round  Server,cmd=[0x%04x]\n", cmd );
	_LOG_DEBUG_( "Recv Packet From Round  Server,seqNo=[0x%04x]\n", seqNo );
*/
	if(cmd==CMD_BACKSVR_HEART)
	{
		_LOG_DEBUG_( "Recv Packet From RoundServer,cmd=[0x%04x]\n", cmd );
		_LOG_DEBUG_( "Recv RoundServer HeartBeat Pong\n", cmd );
		return 0;
	}

	_LOG_DEBUG_( "Recv Packet From RoundServer,cmd=[0x%04x]\n", cmd );
	if(cmd == ROUND_GET_INFO)
	{
		int uid = pPacket->ReadInt();
		short tid = pPacket->ReadShort();
		int win = pPacket->ReadInt();
		int lose = pPacket->ReadInt();
		int tie = pPacket->ReadInt();
		int runaway = pPacket->ReadInt();
		_LOG_INFO_("[RoundSocketHandle] tid[%d] uid[%d] win[%d] lose[%d] tie[%d] runaway[%d]\n", tid, uid, win, lose, tie, runaway);
		Room* room = Room::getInstance();
		Table* table = room->getTable(tid);
		if(table)
		{
			Player* player = table->getPlayer(uid);
			if(player)
			{
				player->nwin = win;
				player->nlose = lose;
				player->nrunaway = runaway;
			}
			else
				_LOG_ERROR_("[RoundSocketHandle] Can't Find player[%d] in Table[%d]\n", uid, tid);
		}
		else
			_LOG_ERROR_("[RoundSocketHandle] Can't Find this Table[%d] player[%d]\n", tid, uid);
		return 0;
	}
	Despatch* game_server = Despatch::getInstance();
	 
	return game_server->doBackResponse(0,pPacket);
}
 

//=============================RoundHeartBeatTimer===============================
 
int RoundHeartBeatTimer::ProcessOnTimerOut()
{
	_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "=========RoundServerConnect RoundHeartBeatTimer===========\n" );
	if(backsrv && backsrv->isActive())
	{
		if(backsrv->sendHeartBeat()==0)
			_LOG_DEBUG_( "Send Heart Beat to Round Server OK\n" );
		else
		{
			_LOG_ERROR_( "Send Heart Beat to Round Server ERROR\n" );
			backsrv->setActive(false);
			backsrv->reconnect();
		}
	}
	else
	{
		_LOG_DEBUG_( "RoundSvrConnect DisConnect, Need ReConnect ...\n" );
		backsrv->reconnect();
	}
	_LOG_DEBUG_( "=======================================\n" );
	StartTimer(30*1000);
	return 0;
}
 
