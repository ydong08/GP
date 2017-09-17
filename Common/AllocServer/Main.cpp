#include <signal.h>
#include <math.h>
#include "CDLReactor.h"
#include "Configure.h"
#include "ClientHandler.h"
#include "Despatch.h"
#include "Logger.h"
#include "SvrConfig_.h"
#include "RedisAccess.h"
#include "BackConnect.h"
#include "ErrorMsg.h"
#include "TrumptConnect.h"
#include "Protocol.h"
#include "Util.h"
#include "ErrorMessage.h"
#include "StrFunc.h"


RedisAccess *RedisConnector = NULL;

int initBackServerConnect()
{
	SvrConfig_& svf = SvrConfig_::getInstance();
	svf.initWithAddr(Configure::getInstance()->m_sUserAddr.c_str());
	for(int i =0; i<svf.getBackNodes();++i)
	{
		Nodes* nodes = svf.getNodes(i);
		BackNodes* backnodes = new BackNodes(nodes->id);
		 
		for(int j=0; j<nodes->count();++j)
		{
			Node* node = nodes->get(j);
			int ret = backnodes->addNode(node);
			if(ret==-1)
			{
				LOGGER(E_LOG_ERROR) << "user nodes id = " << nodes->id
					<< " node id = " << node->id << " " << node->ip << ":" << node->port;
				return -1;
			}
			else
			{
				LOGGER(E_LOG_INFO) << "user nodes connect ok, id = " << nodes->id
					<< " node id = " << node->id << " " << node->ip << ":" << node->port;
			}

		}

		BackConnectManager::addNodes( nodes->id, backnodes );
	}

    return 0;
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(0));
	CDLReactor::Instance()->Init();

	Util::registerSignal();

	if(!Configure::getInstance()->LoadConfig(argc,argv))
	{
		_LOG_ERROR_("Read Cfg fault,System exit\n");
		return -1;
	}

	AddressInfo addrLog;
	Util::parseAddress(Configure::getInstance()->m_sLogAddr.c_str(), addrLog);
	CLogger::InitLogger(
			Configure::getInstance()->m_nLogLevel,
			Configure::getInstance()->GetGameTag(),
			Configure::getInstance()->m_nServerId,
			addrLog.ip,
			addrLog.port
	);

	//启动Server
	AllocServer server(Configure::getInstance()->m_sListenIp.c_str(), Configure::getInstance()->m_nPort);

	if (CDLReactor::Instance()->RegistServer(&server) == -1)
		return -1;

	RedisConnector = new RedisAccess();
	bool ret = RedisConnector->connect(Configure::getInstance()->m_sServerRedisAddr[READ_REDIS_CONF].c_str());
	if (!ret)
	{
		_LOG_ERROR_("Connect Redis Fault\n");
	}
	else
	{
		_LOG_INFO_("Connect Redis Success\n");
	}
	if(initBackServerConnect()<0)
	{
		LOGGER(E_LOG_ERROR) << "Init User Server Connector fault,System exit";
		return -1;
	}

	AddressInfo addrTrumpt;
	Util::parseAddress(Configure::getInstance()->m_sTrumptAddr.c_str(), addrTrumpt);
	if(TrumptConnect::getInstance()->connect(addrTrumpt.ip.c_str(), addrTrumpt.port)==-1)
	{
		LOGGER(E_LOG_ERROR) << "Connect TrumptServer Error," << addrTrumpt.ip << ":" << addrTrumpt.port;
		return -1;
	}

	return CDLReactor::Instance()->RunEventLoop();
}

