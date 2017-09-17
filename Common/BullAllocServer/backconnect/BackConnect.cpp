#include "GameCmd.h"
#include "CDLReactor.h"
#include "Despatch.h"
#include "BackConnect.h"
#include "Logger.h"
#include "Configure.h"

static std::map<int, BackNodes*> _nodes_map;
static InputPacket pPacket;
//=================ConnectHandler=========================
ConnectHandler::ConnectHandler( Connector* connector )
{
	this->connector = connector;
}

ConnectHandler::~ConnectHandler( )
{
	//this->_decode = NULL;
}

static void registAlloc(int id,CDLSocketHandler* handler)
{
    OutputPacket OutPkg;
    OutPkg.Begin(ALLOC_REGIST);
    OutPkg.WriteInt(Configure::instance()->m_nServerId);
    OutPkg.End();
    if(handler && handler->Send(OutPkg.packet_buf(),OutPkg.packet_size())>0 )
        _LOG_INFO_("AllocServer Regist\n");
}

int ConnectHandler::OnConnected()
{
	//_LOG_INFO_("Connect Server[%s:%d] fd=[%d] Success\n",connector->ip, connector->port, netfd);
	LOGGER(E_LOG_INFO) << "Connect Server ip = " << connector->ip << " port = " << connector->port << " success";
	this->connector->isConnected = true;
	registAlloc(1,this);
	return 0;
}

int ConnectHandler::OnClose()
{
	//_LOG_INFO_("Connect Close [%s:%d] fd=[%d] Success\n",connector->ip, connector->port, netfd);
	LOGGER(E_LOG_INFO) << "Close Server ip = " << connector->ip << " port = " << connector->port << " fd = " << netfd << " success";
	this->connector->isConnected = false;
	this->connector->handler = NULL;
	return 0;
}

int ConnectHandler::OnPacketComplete(const char* data, int len)
{
	pPacket.Copy(data,len);
    short cmd = pPacket.GetCmdType() ;

    /*if (pPacket.CrevasseBuffer() == -1)
    {
        _LOG_ERROR_( "[DATA CrevasseBuffer ERROR] data decrypt error \n" );
        return -1;
    }*/

    _LOG_DEBUG_("BackNode Recv Packet Cmd=[0x%04x]\n",cmd);
    return Despatch::getInstance()->doBackResponse(this,&pPacket);
}
	

//====================Connector===================== ===
Connector::Connector(int id)
{
	this->id = id;
	this->isConnected = false;
	this->fd=0;
	this->suspended= false ;
}

 
Connector::~Connector()
{

}

int Connector::connect(const char* ip, short port)
{
	//建立连接
	//strcpy(this->ip, ip);
	this->ip = ip;
	this->port = port;
	ConnectHandler * phandler = new ConnectHandler(this);
	if( CDLReactor::Instance()->Connect(phandler,this->ip.c_str(),port) < 0 )
	{
		delete phandler;
	   _LOG_ERROR_("Connect BackServer error[%s:%d]\n", this->ip.c_str(),port);
	   return -1;
	}
	else
	{
		this->handler = phandler;
		return 0;
	}
}

int Connector::reconnect()
{
	return connect(this->ip.c_str(), this->port);
}

int Connector::send(OutputPacket* outPack, bool isEncrypt)
{
	if(isEncrypt)	
		outPack->EncryptBuffer();
	if(this->handler)
		return this->handler->Send(outPack->packet_buf(),outPack->packet_size());
	else
		return -1;
}

int Connector::send(char* buff, int len)
{
	if(this->handler)
		return this->handler->Send( buff, len );
	else
		return -1;
}


void Connector::heartbeat()
{
	OutputPacket OutPkg;
	OutPkg.Begin(ALLOC_HEART_BEAT);
	OutPkg.WriteInt(Configure::instance()->m_nServerId);
	OutPkg.WriteString("AllocServer HeartBeat"); 
	OutPkg.End();
	this->send(&OutPkg, false);
}
//=================BackNodes=========================
BackNodes::BackNodes(int id)
{
	this->id = id;
	conn_size=0;
	memset(connectors, 0, sizeof(Connector*)*MAX_BACK_CONN);
    this->StartTimer(10000);
}
 
BackNodes::~BackNodes()
{

}

int BackNodes::addNode(Node* node)
{
	return connect(node ->id, node->ip,node ->port);
}
			 

int BackNodes::connect(int ID,char* host, int port)
{
	if(ID<0 || ID>(MAX_BACK_CONN-1))
	{
		return -1;
	}

	Connector * conn = new Connector(ID);
	 
	if (conn->connect(host, port)==-1)
	{
	   delete conn;
	   return -1;
	}
	else
	{
		conn_size ++;
		connectors[ID] = conn;
		return 0;
	}
 }
				 
int BackNodes::send(int id,OutputPacket* outPack, bool isEncrypt )
{
	if(id>0 && id<MAX_BACK_CONN)
	{
		Connector* pconnect = this->connectors[id];
		if( pconnect && pconnect->isActive() && !pconnect->isSuspended() )
			return pconnect->send( outPack, isEncrypt);
	}
	return -1;
}

int BackNodes::send(OutputPacket* outPack, bool isEncrypt)
{
	for(int i=1; i<=conn_size; i++)
	{
		Connector* pconnect = this->connectors[i];
		if( pconnect && pconnect->isActive() && !pconnect->isSuspended())
			return pconnect->send(outPack, isEncrypt);
	}
	return -1;
}

int BackNodes::send(InputPacket* inputPack)
{
	_LOG_DEBUG_("conn_size[%d] send \n",conn_size);
	for(int i=1; i<=conn_size; i++)
	{
		Connector* pconnect = this->connectors[i];
		if( pconnect && pconnect->isActive() && !pconnect->isSuspended())
			return pconnect->send(inputPack->packet_buf(),inputPack->packet_size());
	}
	return -1;
}


void BackConnectManager::addNodes(int id, BackNodes* backnodes)
{
	_nodes_map[id] = backnodes;
}

BackNodes* BackConnectManager::getNodes(int id)
{
	if (_nodes_map.find(id) == _nodes_map.end())
	{
		if (_nodes_map.empty()) return NULL;
		int idx = rand() % _nodes_map.size();
		std::map<int, BackNodes*>::iterator itr = _nodes_map.begin();
		for (int i=0; i<idx; ++i) itr++;
		return itr->second;                     
	}

	return _nodes_map[id];
}
		 
std::map<int, BackNodes*>* BackConnectManager::getNodess()
{
	return &_nodes_map;
}

//=================BackNodes=========================
int BackNodes::ProcessOnTimerOut()
{
	for(int i=1; i<=this->conn_size; ++i)
	{
		Connector* pconnect = this->connectors[i];
		if(pconnect && pconnect->isActive())
		{
			//发心跳包
			pconnect->heartbeat();
			_LOG_DEBUG_("node[%d][%d]:heartbeat\n",  this->id, pconnect->id);
		}
		else
		{
			//重连接
			pconnect->reconnect(); 
			_LOG_INFO_("node[%d][%d]:reconnect\n", this->id, pconnect->id);
		}
	}
	StartTimer(10000);
	return 0;
}

