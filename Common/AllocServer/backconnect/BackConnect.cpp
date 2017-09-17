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
   // this->_decode = NULL;
}

static void registAlloc(int id,CDLSocketHandler* handler)
{
    OutputPacket OutPkg;
    OutPkg.Begin(ALLOC_REGIST);
    OutPkg.WriteInt(Configure::getInstance()->m_nServerId);
    OutPkg.End();
	if (handler && handler->Send(OutPkg.packet_buf(), OutPkg.packet_size()) > 0)
	{
		LOGGER(E_LOG_INFO) << "HallServer Regist";
	}
}

int ConnectHandler::OnConnected()
{
	LOGGER(E_LOG_INFO) << "Connect Server " << connector->ip << ":" << connector->port << " fd = " << netfd;
    this->connector->isConnected = true;
    registAlloc(1,this);
    return 0;
}

int ConnectHandler::OnClose()
{
	LOGGER(E_LOG_INFO) << "Connect Close " << connector->ip << ":" << connector->port << " fd = " << netfd;
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
	LOGGER(E_LOG_DEBUG) << "BackNode Recv Packet Cmd=" << cmd;
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
    strcpy(this->ip, ip);
    this->port = port;
    ConnectHandler * phandler = new ConnectHandler(this);
    if( CDLReactor::Instance()->Connect(phandler,ip,port) < 0 )
    {
        delete phandler;
	   LOGGER(E_LOG_ERROR) << "Connect BackServer error " << ip << ":" << port;
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
    OutPkg.Begin(ALLOC_HEART_BEAT);
    OutPkg.WriteInt(Configure::getInstance()->m_nServerId);
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
    return connect(node ->id, node ->ip,node ->port);
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
	LOGGER(E_LOG_DEBUG) << "conn_size = " << conn_size << " send";
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
    if(id<0)
    {
        return NULL;
    }

    return _nodes_map[id];
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
			LOGGER(E_LOG_DEBUG) << "node id =" << this->id << " connect id = " << pconnect->id;
        }
        else
        {
            //重连接
            pconnect->reconnect();
			LOGGER(E_LOG_DEBUG) << "node id = " << this->id << " pconnect id = " << pconnect->id << " reconnect";
        }
    }
    StartTimer(10000);
	return 0;
}

std::map<int, BackNodes*> * BackConnectManager::getNodess()
{
    return &_nodes_map;
}
