#include <signal.h>
#include "CDLReactor.h"
#include "Configure.h"
//#include "Despatch.h"
#include "Logger.h"
#include "RobotSvrConfig.h"
#include "GameServerConnect.h"
#include "CoinConf.h"
#include "Protocol.h"
#include "NamePool.h"

#ifdef CRYPT
#include "CryptRedis.h"
#endif

void registerSignal()
{
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
}

int get_monitor_time()
{
    short monitortime = Configure::getInstance()->monitortime;
    if(Configure::getInstance()->level == 1)
        monitortime = Configure::getInstance()->monitortime;
    if(Configure::getInstance()->level == 2)
        monitortime = Configure::getInstance()->monitortime + 1;
    if(Configure::getInstance()->level == 3)
        monitortime = Configure::getInstance()->monitortime + 2;
    
    return monitortime;
}

int initGameConnector()
{
	RobotSvrConfig& svf = RobotSvrConfig::getInstance();
	svf.initServerConf(Configure::getInstance()->serverxml.c_str());
	for(int i =0; i<svf.getServerListSize();++i)
	{
		ServerNode* node = svf.getServerNode(i);
        GameConnector* gameserver = new GameConnector(node->svid);
        gameserver->setGetMonitorTime(get_monitor_time);
		 
		if(gameserver && gameserver->connect(node->ip, node->port) !=0)
		{
			_LOG_ERROR_("Svid[%d] Connect BackServer error[%s:%d]\n",node->svid, node->ip,node->port);
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

	if(!Configure::getInstance()->LoadConfig(argc,argv))
	{
		_LOG_ERROR_("Read Cfg fault,System exit");
		return -1;
	}
	AddressInfo addrLog;
	Util::parseAddress(Configure::getInstance()->m_sLogAddr.c_str(), addrLog);
	CLogger::InitLogger(
		Configure::getInstance()->m_nLogLevel,
		Configure::getInstance()->GetGameTag(),
		Configure::getInstance()->m_robotId,
		addrLog.ip,
		addrLog.port
	);

  	if(initGameConnector()<0)
	{
		printf("Init Alloc Server Connector fault,System exit\n");
		return -1;
	}

	if(!CoinConf::getInstance()->init(Configure::getInstance()->m_sServerRedisAddr[READ_REDIS_CONF].c_str())) {
        printf("CoinConf Error\n");
        return -1;
    }

	if(PlayerManager::getInstance()->init()<0)
		return -1;

#ifdef CRYPT
	if (!CCryptRedis::getInstance().init(Configure::getInstance()->m_sPhpRedisAddr[READ_REDIS_CONF].c_str()))
	{
		LOGGER(E_LOG_ERROR) << "Connect Crypt Redis Error,[" << Configure::getInstance()->m_sPhpRedisAddr << "]";
		return -1;
	}
#endif

	return CDLReactor::Instance()->RunEventLoop();
}

