#include <signal.h>
#include "CDLReactor.h"
#include "Configure.h"
#include "Despatch.h"
#include "Logger.h"
#include "SvrConfig.h"
#include "GameServerConnect.h"
#include "RobotRedis.h"
#include "PlayerManager.h"
#include "Protocol.h"
#include "Util.h"
#include "NamePool.h"

int GAME_ID = E_TEXAS_GAME_ID;
short SOURCE = E_MSG_SOURCE_ROBOT;

void registerSignal()
{
	signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
}   


int initGameConnector()
{
	SvrConfig* svf = SvrConfig::getInstance();
	svf->initServerConf();
	for(int i =0; i<svf->getServerListSize();++i)
	{
		ServerNode* node = svf->getServerNode(i);
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

	srand((unsigned int)time(0)^getpid());
	registerSignal();

	if(!Configure::instance()->LoadConfig(argc, argv))
	{
		LOGGER(E_LOG_ERROR) << "ReadConfig Failed.";
		return -1;
	}
	AddressInfo addrLog;
	Util::parseAddress(Configure::instance()->m_sLogAddr.c_str(), addrLog);
	CLogger::InitLogger(Configure::instance()->m_nLogLevel,
						"LandRobotServer",
						Configure::instance()->m_nServerId,
						addrLog.ip,
						addrLog.port);


	RobotServer robotserver(Configure::instance()->m_sListenIp.c_str(), Configure::instance()->m_nPort);

	if (CDLReactor::Instance()->RegistServer(&robotserver) == -1)
		return -1;
	else
		LOGGER(E_LOG_DEBUG) << "RobotServer have been started,listen port:" << Configure::instance()->m_nPort;
	
    if(RobotRedis::getInstance()->InitRedis() != 0)
    {
		printf("Can't Link RobotRedis Server \n");
		return -1;
    }
	if(initGameConnector()<0)
	{
		printf("Init Alloc Server Connector fault,System exit\n");
		return -1;
	}
    NamePool::getInstance()->Initialize("../conf/names.txt");

	if(PlayerManager::getInstance()->init()<0)
		return -1;
    SvrConfig::getInstance()->initLinkConf();
    SvrConfig::getInstance()->initNameConf();
	return CDLReactor::Instance()->RunEventLoop();
}

