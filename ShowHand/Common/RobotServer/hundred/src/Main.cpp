#include <signal.h>
#include "CDLReactor.h"
#include "Configure.h"
#include "Despatch.h"
#include "Logger.h"
#include "Version.h"
#include "RobotSvrConfig.h"
#include "GameServerConnect.h"
#include "CoinConf.h"
#include "Protocol.h"
#include "NamePool.h"
#include "Util.h"


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
	RobotSvrConfig& svf = RobotSvrConfig::getInstance();
	svf.initServerConf(Configure::getInstance()->serverxml.c_str());
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

	srand((unsigned int)time(0)^getpid());
	registerSignal();

	if (!Configure::getInstance()->LoadConfig(argc, argv))
	{
		LOGGER(E_LOG_ERROR) << "ReadConfig Failed.";
		return -1;
	}
	//__LOG_INIT__(Configure::getInstance()->loglevel, Configure::getInstance()->logfile );
	AddressInfo addrLog;
	Util::parseAddress(Configure::getInstance()->m_sLogAddr.c_str(), addrLog);
	CLogger::InitLogger(
		Configure::getInstance()->m_nLogLevel,
		Configure::getInstance()->GetGameTag(),
		Configure::getInstance()->m_nRobotIndex,
		addrLog.ip,
		addrLog.port
	);

	NamePool::getInstance()->Initialize("../conf/names.txt");
	RobotServer robotserver(Configure::getInstance()->m_sListenIp.c_str(),Configure::getInstance()->m_nPort);

	if (CDLReactor::Instance()->RegistServer(&robotserver) == -1)
		return -1;
	else
		LOGGER(E_LOG_DEBUG) << "RobotServer have been started,listen port:" << Configure::getInstance()->m_nPort;
	
	if(initGameConnector()<0)
	{
		printf("Init Alloc Server Connector fault,System exit\n");
		return -1;
	}
	if(CoinConf::getInstance()->init() != 0 )
		printf("CoinConf Error\n");
	if(PlayerManager::getInstance()->init()<0)
		return -1;
	return CDLReactor::Instance()->RunEventLoop();
}

