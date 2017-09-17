#include "GameCmd.h"
#include "CDLReactor.h"
#include "Despatch.h"
#include "GameServerConnect.h"
#include "Logger.h"
#include "Options.h"
#include "PlayerManager.h"

using namespace std;

static map<int, GameConnector*> _gameserver_map;
static InputPacket pPacket;

//=================GameConnectHandler=========================
GameConnectHandler::GameConnectHandler( GameConnector* connector )
{
	this->connector = connector;
}

GameConnectHandler::~GameConnectHandler( )
{
	//this->_decode = NULL;
}

int GameConnectHandler::OnConnected()
{
	LOGGER(E_LOG_INFO) << "Connect Server " << connector->ip << ":" << connector->port << " netfd = " << netfd;
	this->connector->isConnected = true;
	return 0;
}

int GameConnectHandler::OnClose()
{
	LOGGER(E_LOG_INFO) << "Connect Close " << connector->ip << ":" << connector->port << " netfd = " << netfd;
	this->connector->isConnected = false;
	this->connector->handler = NULL;
	return 0;
}

int GameConnectHandler::OnPacketComplete(const char* data, int len)
{
	pPacket.Copy(data,len);
    short cmd = pPacket.GetCmdType() ;

	LOGGER(E_LOG_DEBUG) << "BackNode Recv Packet Cmd = " << cmd;
    return Despatch::getInstance()->doBackResponse(this,&pPacket);
}
	

//====================GameConnector===================== ===
GameConnector::GameConnector(int id)
{
	this->svid = id;
	this->isConnected = false;
	this->suspended= false ;
}

GameConnector::~GameConnector()
{

}

int GameConnector::connect(const char* ip, short port)
{
	//建立连接
	strcpy(this->ip, ip);
	this->port = port;
	GameConnectHandler * handler = new GameConnectHandler(this);
	if( CDLReactor::Instance()->Connect(handler,ip,port) < 0 )
	{
		delete handler;
		LOGGER(E_LOG_ERROR) << "Connect BackServer error " << ip << ":" << port;
	   return -1;
	}
	else
	{
		this->handler = handler;
		this->ScanningServer();
		short monitortime = Options::instance()->monitortime;
		StartTimer( monitortime * 1000);
		return 0;
	}
}

int GameConnector::reconnect()
{
	return connect(this->ip, this->port);
}

int GameConnector::send(OutputPacket* outPack, bool isEncrypt)
{
	if(isEncrypt)	
		outPack->EncryptBuffer();
	if(this->handler)
		return this->handler->Send(outPack->packet_buf(),outPack->packet_size());
	else
		return -1;
}

int GameConnector::send(InputPacket* inPack)
{
	if(this->handler)
		return this->handler->Send(inPack->packet_buf(),inPack->packet_size());
	else
		return -1;
}

int GameConnector::send(char* buff, int len)
{
	if(this->handler)
		return this->handler->Send( buff, len );
	else
		return -1;
}

void GameConnector::ScanningServer()
{
	OutputPacket OutPkg;
	OutPkg.Begin(ADMIN_GET_IDLE_TABLE);
	OutPkg.End();
	this->send(&OutPkg);
}

int GameConnector::ProcessOnTimerOut()
{
	//_LOG_INFO_("Player size[%d] level[%d]\n", PlayerManager::getInstance()->getPlayerSize(),Options::instance()->level);
	LOGGER(E_LOG_INFO) << "Player size[" << PlayerManager::getInstance()->getPlayerSize() << "] level[" << Options::instance()->level<< "]";
	if(this->isActive())
	{
		this->ScanningServer();
	}
	else
	{
		this->reconnect();
	}
	StartTimer(Options::instance()->monitortime * 1000);
}
		 
//=================BackGameConnectManager=========================

void BackGameConnectManager::addGameNode(int svid, GameConnector* backgamenode)
{
	_gameserver_map[svid] = backgamenode;
}

GameConnector* BackGameConnectManager::getGameNode(int svid)
{
	map<int, GameConnector*>::iterator iter = _gameserver_map.find(svid);
	if(iter != _gameserver_map.end())
	{
		return iter->second;
	}
	return NULL;
}

void BackGameConnectManager::delGameNode(int svid)
{
	_gameserver_map.erase(svid);
}

