#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "Configure.h"
#include "ClientHandler.h"
#include "Despatch.h"
#include "Logger.h"
#include "AllocSvrConnect.h"
#include "CDLReactor.h"
#include "Room.h"
#include "CoinConf.h"
#include "ErrorMsg.h"
#include "MySqlConnect.h"
#include "LogServerConnect.h"
#include "UdpManager.h"
#include "TaskManager.h"
#include "SvrConfig.h"
#include "MoneyConnect.h"
#include "RoundServerConnect.h"
#include "TaskRedis.h"
#include "WordServerConnect.h"
#include "Version.h"
#include "OperationRedis.h"
#include "ErrorMessage.h"
#include "OfflineRedis.h"

int GAME_ID = E_SHOWHAND_GAME_ID;
short SOURCE = E_MSG_SOURCE_SERVER;

MyLogger* logger = new MyLogger();

void registerSignal()
{
	signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
} 

int initMoneyServerConnect()
{
	SvrConfig* svf = SvrConfig::getInstance();
	for(int i =0; i<svf->getBackNodes();++i)
	{
		Nodes* nodes = svf->getNodes(i);
		MoneyNodes* backnodes = new MoneyNodes(nodes->id);
		 
		for(int j=0; j<nodes->count();++j)
		{
			Node* node = nodes->get(j);
			int ret = backnodes->addNode(node);
			if (ret == -1)
			{
				//_LOG_ERROR_("Nodes[%d] node[%d][%s:%d] Connected Fault\n",nodes->id,node ->id,node ->ip,node ->port);
				LOGGER(E_LOG_DEBUG) << "Nodes[" << nodes->id << "] node[" << node->id << "][" << node->ip << ":" << node->port << "] Connected Fault\n";
				return -1;
			}
			else
				//_LOG_INFO_("Nodes[%d] node[%d][%s:%d] Connected OK\n",nodes->id,node ->id,node ->ip,node ->port);
				LOGGER(E_LOG_INFO) << "Nodes[" << nodes->id << "] node[" << node->id << "][" << node->ip << ":" << node->port << "] Connected OK\n";

		}

		MoneyConnectManager::addNodes( nodes->id, backnodes );
	}
    return 0;
}

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL)^getpid());
	registerSignal();
	//printf("Room size:%d\n", sizeof(Room));

	CDLReactor::Instance()->Init();


	if (Configure::getInstance()->parse_args(argc, argv) != 0)
		return -1;
	if (Configure::getInstance()->read_conf("../conf/showhand.ini") != 0)
		return -1;
	Configure::getInstance()->printConf();

	CLogger::InitLogger(
		Configure::getInstance()->m_loglevel,
		"ShowhandFight",
		Configure::getInstance()->server_id,
		Configure::getInstance()->m_logRemoteIp,
		Configure::getInstance()->m_logRemotePort
	);
	//__LOG_INIT__(Configure::getInstance()->loglevel, Configure::getInstance()->logfile );

	ErrorMsg::getInstance()->initFromBuf(g_errorMsg);
	
	Room::getInstance()->init();
	
	if(CoinConf::getInstance()->InitRedis() != 0 )
		printf("CoinConf Error\n");
	//私人房和二人场不下发任务
	if(Configure::getInstance()->level < 4)
	{
		if(TaskManager::getInstance()->init() != 0 )
		{
			printf("TaskManager is Error!\n");
			return -1;
		}
	}

	GameAcceptor gameAcceptor(Configure::getInstance()->listen_address,Configure::getInstance()->port);

	if (CDLReactor::Instance()->RegistServer(&gameAcceptor) == -1)
		return -1;
	else
		//_LOG_DEBUG_("ShowHand[%s.%s] have been started,listen port:%d...\n",VERSION,SUBVER,Configure::getInstance()->port);
		LOGGER(E_LOG_DEBUG) << "ShowHand[" << VERSION << "." << SUBVER << "] have been started,listen port:" << Configure::getInstance()->port << "...\n";

	if(RoundServerConnect::getInstance()->connect(Configure::getInstance()->round_ip,Configure::getInstance()->round_port)==-1)
	{
		//_LOG_ERROR_("Connect RoundServer Error,[%s:%d]\n",Configure::getInstance()->mysql_ip,Configure::getInstance()->mysql_port);
		LOGGER(E_LOG_ERROR) << "Connect RoundServer Error,[" << Configure::getInstance()->mysql_ip << ":" << Configure::getInstance()->mysql_port << "]\n";
		return -1;
	}


// 	if(LogServerConnect::getInstance()->connect(Configure::getInstance()->log_server_ip,Configure::getInstance()->log_server_port)==-1)
// 	{
// 		//_LOG_ERROR_("Connect LogServer Error,[%s:%d]\n",Configure::getInstance()->log_server_ip,Configure::getInstance()->log_server_port);
// 		LOGGER(E_LOG_ERROR) << "Connect LogServer Error,[" << Configure::getInstance()->log_server_ip << ":" << Configure::getInstance()->log_server_port << "]\n";
// 		return -1;
// 	}

	if(MySqlConnect::getInstance()->connect(Configure::getInstance()->mysql_ip,Configure::getInstance()->mysql_port)==-1)
	{
		//_LOG_ERROR_("Connect MysqlServer Error,[%s:%d]\n",Configure::getInstance()->mysql_ip,Configure::getInstance()->mysql_port);
		LOGGER(E_LOG_ERROR) << "Connect MysqlServer Error,[" << Configure::getInstance()->mysql_ip << ":" << Configure::getInstance()->mysql_port << "]\n";
		return -1;
	}
	
	AllocSvrConnect* alloc = AllocSvrConnect::getInstance();
	if(alloc->connect(Configure::getInstance()->alloc_ip,Configure::getInstance()->alloc_port)==-1)
	{
		//_LOG_ERROR_("Connect AllocSvr Error,[%s:%d]\n",Configure::getInstance()->alloc_ip,Configure::getInstance()->alloc_port);
		LOGGER(E_LOG_ERROR) << "Connect AllocSvr Error,[" << Configure::getInstance()->alloc_ip << ":" << Configure::getInstance()->alloc_port << "]\n";
		return -1;
	}

	WordServerConnect* wordServerConnect = WordServerConnect::getInstance();
	if(wordServerConnect->connect(Configure::getInstance()->word_server_ip,Configure::getInstance()->word_server_port)==-1)
	{
		//_LOG_ERROR_("Connect WordServer Error,[%s:%d]\n",Configure::getInstance()->word_server_ip,Configure::getInstance()->word_server_port);
		LOGGER(E_LOG_ERROR) << "Connect WordServer Error,[" << Configure::getInstance()->word_server_ip << ":" <<Configure::getInstance()->word_server_port << "]\n";
	}
	else
	{
		//_LOG_INFO_("Connect WordServer OK\n");
		LOGGER(E_LOG_INFO) << "Connect WordServer OK\n";
	}

	if(initMoneyServerConnect()<0)
	{
		//_LOG_ERROR_("Init Money Server Connector fault,System exit\n");
		LOGGER(E_LOG_ERROR) << "Init Money Server Connector fault,System exit\n";
		return -1;
	}

	if(TaskRedis::getInstance()->InitRedis() != 0 )
	{
		printf("TaskRedis is Error!\n");
		return -1;
	}

	if(UdpManager::getInstance().init(Configure::getInstance()->udp_ip,Configure::getInstance()->udp_port, GAME_ID)==-1)
	{
		//_LOG_ERROR_("udp init Error,[%s:%d]\n",Configure::getInstance()->udp_ip,Configure::getInstance()->udp_port);
		LOGGER(E_LOG_ERROR) << "udp init Error, [" << Configure::getInstance()->udp_ip << ":" << Configure::getInstance()->udp_port << "]\n";
		return -1;
	}

    if (!OfflineRedis::getInstance()->Initialize(Configure::getInstance()->offline_redis_ip, Configure::getInstance()->offline_redis_port))
    {
        _LOG_ERROR_("Connect Offline Redis Error,[%s:%d]\n", Configure::getInstance()->offline_redis_ip, Configure::getInstance()->offline_redis_port);
        return -1;
    }

	if (!OperationRedis::getInstance()->Initialize(Configure::getInstance()->operation_redis_ip, Configure::getInstance()->operation_redis_port))
	{
		_LOG_ERROR_("Connect Operation Redis Error,[%s:%d]\n", Configure::getInstance()->operation_redis_ip, Configure::getInstance()->operation_redis_port);
		return -1;
	}
	return CDLReactor::Instance()->RunEventLoop();
}
