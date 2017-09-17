#include <signal.h>
#include "CDLReactor.h"
#include "Configure.h"
#include "Logger.h"
#include "Version.h"
#include "RobotSvrConfig.h"
#include "GameServerConnect.h"
#include "ClientHandler.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "Protocol.h"
#include "Packet.h"
#include "Util.h"
#include "NamePool.h"


int initGameConnector()
{
	RobotSvrConfig& svf = RobotSvrConfig::getInstance();
	svf.initServerConf(Configure::getInstance().serverxml.c_str());
	for(int i =0; i<svf.getServerListSize();++i)
	{
		ServerNode* node = svf.getServerNode(i);
		GameConnector* gameserver = new GameConnector(node->svid);
		 
		if(gameserver && gameserver->connect(node->ip, node->port) !=0)
		{
			LOGGER(E_LOG_ERROR) << "Svid[" << node->svid << "] Connect BackServer error[" << node->ip << ":" << node->port;
			delete gameserver;
			exit(1);
		}
		BackGameConnectManager::addGameNode(node->svid, gameserver);
	}
    return 0;
}

int main(int argc, char* argv[])
{
	CDLReactor::Instance()->Init();

#ifndef WIN32
	srand((unsigned int)time(0)^getpid());
	Util::registerSignal();
#endif

	if(!Configure::getInstance().LoadConfig(argc, argv))
	{
		LOGGER(E_LOG_ERROR) << "ReadConfig Failed.";
		return -1;
	}
    AddressInfo addrLog;
    Util::parseAddress(Configure::getInstance().m_sLogAddr.c_str(), addrLog);
    CLogger::InitLogger(Configure::getInstance().m_nLogLevel,
                        Configure::getInstance().GetGameTag(),
                        Configure::getInstance().id,
                        addrLog.ip,
                        addrLog.port);

	NamePool::getInstance()->Initialize("../conf/names.txt");

// 	RobotServer robotserver(Configure::getInstance().m_sListenIp.c_str(), Configure::getInstance().m_nPort);
// 	if (CDLReactor::Instance()->RegistServer(&robotserver) == -1)
// 	{
// 		LOGGER(E_LOG_ERROR) << "register server failed!";
// 		return -1;
// 	}
// 	else
// 	{
// 		LOGGER(E_LOG_DEBUG) << "RobotServer have been started,listen port:" << Configure::getInstance().port;
// 	}
    
	if(initGameConnector()<0)
	{
		LOGGER(E_LOG_ERROR) << "Init Alloc Server Connector fault,System exit!";
		return -1;
	}
	return CDLReactor::Instance()->RunEventLoop();
}

