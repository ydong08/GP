#include <signal.h>
#include "CDLReactor.h"
#include "Despatch.h"
#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"
#include "RobotSvrConfig.h"
#include "GameServerConnect.h"
#include "Util.h"
#include "NamePool.h"

short SOURCE = E_MSG_SOURCE_ROBOT;


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
	
	Util::registerSignal();
	if (!Configure::getInstance().LoadConfig(argc, argv))
	{
		LOGGER(E_LOG_WARNING) << "Read Cfg fault,System exit";
		return -1;
	}

	AddressInfo addrLog;
	Util::parseAddress(Configure::getInstance().m_sLogAddr.c_str(), addrLog);
	CLogger::InitLogger(
		Configure::getInstance().m_nLogLevel,
		"FlowerFightRobot",
		Configure::getInstance().m_nServerId,
		addrLog.ip,
		addrLog.port
		);

	NamePool::getInstance()->Initialize("../conf/names.txt");

	LOGGER(E_LOG_INFO) << "robot head link=" << Configure::getInstance().m_headLink;

	if (initGameConnector()<0)
	{
		LOGGER(E_LOG_ERROR) << "Init Alloc Server Connector fault,System exit";
		return -1;
	}
	return CDLReactor::Instance()->RunEventLoop();
}

