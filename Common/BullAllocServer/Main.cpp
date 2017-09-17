#include <signal.h>
#include <math.h>
#include "CDLReactor.h"
#include "Configure.h"
#include "ClientHandler.h"
#include "Despatch.h"
#include "Logger.h"
#include "Version.h"
#include "SvrConfig_.h"
#include "RedisAccess.h"
#include "ErrorMsg.h"
//#include "UdpManager.h"
#include "TrumptConnect.h"
#include "Packet.h"
#include "Util.h"
//#include "ErrorMessage.h"
#include "Protocol.h"
//#include "ErrorMessage.h"
#include "ServerManager.h"
#include "Packet.h"
#include "BackConnect.h"

RedisAccess *RedisConnector = NULL;


int initBackServerConnect()
{
	SvrConfig_& svf = SvrConfig_::getInstance();
	svf.initWithAddr(Configure::instance()->m_sUserAddr.c_str());
	for (int i = 0; i<svf.getBackNodes(); ++i)
	{
		Nodes* nodes = svf.getNodes(i);
		BackNodes* backnodes = new BackNodes(nodes->id);
		 
		for(int j=0; j<nodes->count();++j)
		{
			Node* node = nodes->get(j);
			int ret = backnodes->addNode(node);
			if(ret==-1)
			{
				_LOG_ERROR_("Nodes[%d] node[%d][%s:%d] Connected Fault\n", nodes->id, node->id, node->ip, node->port);
				return -1;
			}
			else
				_LOG_INFO_("Nodes[%d] node[%d][%s:%d] Connected OK\n", nodes->id, node->id, node->ip, node->port);

		}

		BackConnectManager::addNodes( nodes->id, backnodes );
	}
	_LOG_INFO_("Init Back Server Connect OK \n");

    return 0;
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(0));
	CDLReactor::Instance()->Init();

	Util::registerSignal();

	if (!Configure::instance()->LoadConfig(argc, argv))
	{
		_LOG_ERROR_("Read Cfg fault,System exit\n");
		return -1;
	}

	AddressInfo addrLog;
	Util::parseAddress(Configure::instance()->m_sLogAddr.c_str(), addrLog);
	CLogger::InitLogger(
		Configure::instance()->m_nLogLevel,
		Configure::instance()->GetGameTag(),
		Configure::instance()->m_nServerId,
		addrLog.ip,
		addrLog.port
	);
    
	//启动Server
	AllocServer server(Configure::instance()->m_sListenIp.c_str(), Configure::instance()->m_nPort);

	if (CDLReactor::Instance()->RegistServer(&server) == -1)
		return -1;
	else
		_LOG_DEBUG_("%s[%s.%s] have been started,listen port:%d...\n",Configure::instance()->ServerName(),VERSION,SUBVER,Configure::instance()->m_nPort);

	RedisConnector = new RedisAccess();
	bool ret = RedisConnector->connect(Configure::instance()->m_sServerRedisAddr[READ_REDIS_CONF].c_str());
	if(!ret)
	{
		_LOG_ERROR_("Connect Redis Fault\n");
	}
	else
	{
		_LOG_INFO_("Connect Redis Success\n");
	}
	if(initBackServerConnect()<0)
	{
		_LOG_ERROR_("Init Back Server Connector fault,System exit\n");
		return -1;
	}

	AddressInfo addrTrumpt;
	Util::parseAddress(Configure::instance()->m_sTrumptAddr.c_str(), addrTrumpt);
	if(TrumptConnect::getInstance()->connect(addrTrumpt.ip.c_str(), addrTrumpt.port)==-1)
	{
		_LOG_ERROR_("Connect TrumptServer Error,[%s:%d]\n", addrTrumpt.ip.c_str(), addrTrumpt.port);
		return -1;
	}

	return CDLReactor::Instance()->RunEventLoop();
}

