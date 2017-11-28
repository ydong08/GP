#include <signal.h>
#include "CDLReactor.h"
#include "Options.h"
#include "Despatch.h"
#include "Logger.h"
#include "Version.h"
#include "SvrConfig.h"
#include "GameServerConnect.h"
#include "RobotRedis.h"
#include "PlayerManager.h"
#include "VerifyRedis1.h"
#include "VerifyRedis2.h"
#include "Protocol.h"

MyLogger* logger = new MyLogger();
short SOURCE = E_MSG_SOURCE_ROBOT;
int GAME_ID = E_SHOWHAND_GAME_ID;

void registerSignal()
{
	signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
}   

int readSysCfg(int argc, char *argv[])
{
    if (Options::instance()->parse_args(argc, argv) != 0)
        return -1;
    if (Options::instance()->read_conf("../conf/robot.ini") != 0)
        return -2;

    Options::instance()->printConf();
    return 0;
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

	if(readSysCfg(argc,argv) < 0)
	{
		LOGGER(E_LOG_ERROR) << "ReadConfig Failed.";
		return -1;
	}

	CLogger::InitLogger(
		Options::instance()->m_loglevel,
		"ShowhandFightRobot",
		Options::instance()->m_robotId,
		Options::instance()->m_logRemoteIp,
		Options::instance()->m_logRemotePort
	);
	//__LOG_INIT__(Options::instance()->loglevel, Options::instance()->logfile );

	RobotServer robotserver(Options::instance()->listen_address,Options::instance()->port);

	if (CDLReactor::Instance()->RegistServer(&robotserver) == -1)
		return -1;
	else
		//_LOG_DEBUG_("RobotServer[%s.%s] have been started,listen port:%d...\n",VERSION,SUBVER,Options::instance()->port);
		LOGGER(E_LOG_DEBUG) << "RobotServer[" << VERSION<<"."<<SUBVER<<"] have been started,listen port:"<<Options::instance()->port;

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
	SvrConfig::getInstance()->initLinkConf();
	SvrConfig::getInstance()->initNameConf();
	if(PlayerManager::getInstance()->init()<0)
		return -1;
	
	if(VerifyRedis1::getInstance()->InitRedis() != 0 )
	{
		printf("VerifyRedis1 is Error!\n");
		return -1;
	}
	if(VerifyRedis2::getInstance()->InitRedis() != 0 )
	{
		printf("VerifyRedis2 is Error!\n");
		return -1;
	}

	Options::instance()->m_headLink = RobotRedis::getInstance()->readHeadUrl();
	return CDLReactor::Instance()->RunEventLoop();
}

