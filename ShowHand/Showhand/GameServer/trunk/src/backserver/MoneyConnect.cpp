#include "GameCmd.h"
#include "CDLReactor.h"
#include "Despatch.h"
#include "MoneyConnect.h"
#include "Logger.h"
#include "Configure.h"

static std::map<int, MoneyNodes*> _nodes_map;
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

static void registServer(int id,CDLSocketHandler* handler)
{
    OutputPacket OutPkg;
    OutPkg.Begin(SERVER_REGIST);
    OutPkg.WriteInt(Configure::getInstance()->server_id);
    OutPkg.End();
    if(handler && handler->Send(OutPkg.packet_buf(),OutPkg.packet_size())>0 )
        _LOG_INFO_("Server Regist\n");
}

int ConnectHandler::OnConnected()
{
	_LOG_INFO_("Connect Server[%s:%d] fd=[%d] Success\n",connector->ip, connector->port, netfd);
	this->connector->isConnected = true;
	registServer(1, this);
	return 0;
}

int ConnectHandler::OnClose()
{
	_LOG_INFO_("Connect Close [%s:%d] fd=[%d] Success\n",connector->ip, connector->port, netfd);
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

	if(cmd == UPDATE_MONEY || cmd == UPDATE_ROLL || cmd == UPDATE_COIN)
		return 0;

    _LOG_DEBUG_("MoneyNode Recv Packet Cmd=[0x%04x]\n",cmd);
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
	strcpy(this->ip, ip);
	this->port = port;
	ConnectHandler * phandler = new ConnectHandler(this);
	if( CDLReactor::Instance()->Connect(phandler,ip,port) < 0 )
	{
		delete phandler;
	   _LOG_ERROR_("Connect MoneyServer error[%s:%d]\n",ip,port);
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
	return connect(this->ip, this->port);
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
	OutPkg.Begin(CMD_BACKSVR_HEART);
	OutPkg.End();
	this->send(&OutPkg,false);
}


//=================MoneyNodes=========================
MoneyNodes::MoneyNodes(int id)
{
	this->id = id;
	conn_size=0;
	memset(connectors, 0, sizeof(Connector*)*MAX_BACK_CONN);
    this->StartTimer(30000);
}
 
MoneyNodes::~MoneyNodes()
{

}

int MoneyNodes::addNode(Node* node)
{
	return connect(node ->id, node ->ip,node ->port);
}
			 

int MoneyNodes::connect(int ID,char* host, int port)
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
		//printf("MoneyNodes[%d] add node[%d]\n",this->id, ID);
		conn_size ++;
		connectors[ID] = conn;
		return 0;
	}
	return 0;
}
				 
int MoneyNodes::send(int id,OutputPacket* outPack, bool isEncrypt )
{
	if(id>0 && id<MAX_BACK_CONN)
	{
		Connector* pconnect = this->connectors[id];
		if( pconnect && pconnect->isActive() && !pconnect->isSuspended() )
			return pconnect->send( outPack, isEncrypt);
	}

	return -1;
}

int MoneyNodes::send(OutputPacket* outPack, bool isEncrypt)
{
	for(int i=1; i<=conn_size; i++)
	{
		Connector* pconnect = this->connectors[i];
		if( pconnect && pconnect->isActive() && !pconnect->isSuspended() )
			return pconnect->send(outPack, isEncrypt);
	}
	return -1;
}

int MoneyNodes::send(InputPacket* inputPack)
{
	_LOG_DEBUG_("conn_size[%d] send \n",conn_size);
	for(int i=1; i<=conn_size; i++)
	{
		Connector* pconnect = this->connectors[i];
		if( pconnect && pconnect->isActive() && !pconnect->isSuspended() )
			return pconnect->send(inputPack->packet_buf(),inputPack->packet_size());
	}
	return -1;
}

int MoneyNodes::send(int id,InputPacket* inputPack)
{
	if(id>0 && id<MAX_BACK_CONN)
	{
		Connector* pconnect = this->connectors[id];
		if( pconnect && pconnect->isActive() && !pconnect->isSuspended() )
			return pconnect->send(inputPack->packet_buf(),inputPack->packet_size());
	}
	return -1;
}

int MoneyNodes::sendRandnode(InputPacket* inputPack)
{
	int nodeid = rand()%conn_size + 1;
	return send(nodeid, inputPack);
}

int MoneyNodes::sendRandnode(OutputPacket* outPack, bool isEncrypt)
{
	int nodeid = rand()%conn_size + 1;
	return send(nodeid, outPack, isEncrypt);
}


void MoneyConnectManager::addNodes(int id, MoneyNodes* backnodes)
{
	_nodes_map[id] = backnodes;
}

MoneyNodes* MoneyConnectManager::getNodes(int id)
{
	if (_nodes_map.empty()) return NULL;
	if (id >= _nodes_map.size()) return _nodes_map.begin()->second;
	return _nodes_map[id];
}

//=================MoneyNodes=========================
int MoneyNodes::ProcessOnTimerOut()
{
	for(int i=1; i<=this->conn_size; ++i)
	{
		Connector* pconnect = this->connectors[i];
		if(pconnect && pconnect->isActive())
		{
			pconnect->heartbeat();
			_LOG_DEBUG_("node[%d][%d]:heartbeat\n",  this->id, pconnect->id);
		}
		else
		{
			pconnect->reconnect(); 
			_LOG_DEBUG_("node[%d][%d]:reconnect\n", this->id, pconnect->id);
		}
	}
	StartTimer(30000);
}

