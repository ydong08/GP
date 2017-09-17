#include <signal.h>
#include "CDLReactor.h"
#include "Configure.h"
#include "Despatch.h"
#include "Logger.h"
#include "Version.h"
#include "RobotSvrConfig.h"
#include "GameServerConnect.h"
#include "CoinConf.h"
//#include "RobotRedis.h"
#include "Util.h"
#include "Protocol.h"
#include "NamePool.h"

#ifdef CRYPT
#include "CryptRedis.h"
#endif

int initGameConnector(int32_t robotSvrId)
{
	RobotSvrConfig& svf = RobotSvrConfig::getInstance();
	svf.initServerConf(Configure::getInstance()->serverxml.c_str());

	for (int i = 0, max = svf.getServerListSize(); i < max; ++i)
	{
		ServerNode* node = svf.getServerNode(i);
		if (node == NULL)
		{
			continue;
		}

		// 通过RobotSvrId, 间接地指定需要连接的目标GameSvr的等级
		if (node->svid != robotSvrId)
		{
			continue;
		}

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

	srand((unsigned int)time(0)^getpid());
	Util::registerSignal();

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
		"BullHundredRobot",
		Configure::getInstance()->m_nServerId,
		addrLog.ip,
		addrLog.port
	);

	NamePool::getInstance()->Initialize("../conf/names.txt");

	if(initGameConnector(Configure::getInstance()->m_nServerId)<0)
	{
		LOGGER(E_LOG_ERROR) << "Init Alloc Server Connector fault,System exit\n";
		return -1;
	}

#ifdef CRYPT
	if (!CCryptRedis::getInstance().init(Configure::getInstance()->m_sPhpRedisAddr[WRITE_REDIS_CONF].c_str()))
	{
		LOGGER(E_LOG_ERROR) << "Connect Crypt Redis Error," << Configure::getInstance()->m_sPhpRedisAddr[WRITE_REDIS_CONF];
		return -1;
	}
#endif
	
	if(CoinConf::getInstance()->init() != 0 )
		LOGGER(E_LOG_ERROR) << "CoinConf Error\n";

	if(PlayerManager::getInstance()->init()<0)
		return -1;

	return CDLReactor::Instance()->RunEventLoop();
}

