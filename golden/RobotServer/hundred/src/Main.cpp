#include <signal.h>
#include "CDLReactor.h"
#include "Configure.h"
#include "Logger.h"
#include "RobotSvrConfig.h"
#include "GameServerConnect.h"
#include "CoinConf.h"
#include "Protocol.h"
#include "NamePool.h"

#ifdef CRYPT
#include "CryptRedis.h"
#endif

#ifndef WIN32
void registerSignal()
{
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
}
#endif 

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
			LOGGER(E_LOG_WARNING)<<"Svid["<<node->svid<<"] Connect BackServer error["<<node->ip<<":"<<node->port<<"]";
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
	registerSignal();
#endif

	if (!Configure::getInstance().LoadConfig(argc, argv))
	{
		LOGGER(E_LOG_WARNING)<<"Read Cfg fault,System exit";
		return -1;
	}
	
	AddressInfo addrLog;
	Util::parseAddress(Configure::getInstance().m_sLogAddr.c_str(), addrLog);
	CLogger::InitLogger(
		Configure::getInstance().m_nLogLevel,
		"FlowerHundredRobot",
		Configure::getInstance().m_nServerId,
		addrLog.ip,
		addrLog.port
	);

	NamePool::getInstance()->Initialize("../conf/names.txt");
	CoinConf::getInstance()->init();

	if(initGameConnector()<0)
	{
		LOGGER(E_LOG_ERROR) << "Init Alloc Server Connector fault,System exit";
		return -1;
	}

#ifdef CRYPT
	if (!CCryptRedis::getInstance().init(Configure::getInstance().m_sPhpRedisAddr[WRITE_REDIS_CONF].c_str()))
	{
		LOGGER(E_LOG_ERROR) << "Connect Crypt Redis Error," << Configure::getInstance().m_sPhpRedisAddr[WRITE_REDIS_CONF];
		return -1;
	}
#endif

	return CDLReactor::Instance()->RunEventLoop();
}

