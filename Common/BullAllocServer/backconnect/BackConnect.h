#ifndef __BackConnect_H__
#define __BackConnect_H__

#include <map> 
#include "CDL_Timer_Handler.h"
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"
#include "SvrConfig_.h"

const int  MAX_BACK_CONN = 128;

class BackNodes;
class Connector;

class ConnectHandler :public CDLSocketHandler 
{
	public:
		ConnectHandler(Connector* connector);
		virtual ~ConnectHandler();
        int OnConnected() ;
        int OnClose();
        int OnPacketComplete(const char* data, int len);
		
		CDL_Decoder* CreateDecoder()
    	{
			return &DLDecoder::getInstance();
    	}

	private:
		Connector * connector;
		
};

class Connector
{
	public:
		
		Connector(int id);
		virtual ~Connector();
		int connect(const char* ip, short port);
		int reconnect();
		inline bool isActive(){return this->isConnected;};
		inline bool isSuspended(){return this->suspended;};
		inline void setSuspended(bool b){  this->suspended = b;};
		void heartbeat();
		int send(OutputPacket* outPack, bool isEncrypt=true);
		int send(char* buff, int len);

	private:
		ConnectHandler* handler;
		bool isConnected;
		bool suspended;
		//char ip[128];
		std::string ip;
		short port;
		int id;
		int fd;

	friend class ConnectHandler;
	friend class BackNodes;
	friend class HeartTimer;
 
};

class BackNodes: public CCTimer 
{
        public:
				BackNodes(int id);
                virtual ~BackNodes();
				int addNode(Node* node);
				int connect(int id, char* host, int port);
				int send(int id,OutputPacket* outPack, bool isEncrypt=true); 
				int send(OutputPacket* outPack, bool isEncrypt=true); 
				int send(InputPacket* inputPack); 
		private:
				virtual int ProcessOnTimerOut();
				Connector* connectors[MAX_BACK_CONN];
				int conn_size; 
				int id;
};

class BackConnectManager
{
	public:
		static void addNodes(int id, BackNodes* backnodes);
		static BackNodes* getNodes(int id);
		static std::map<int, BackNodes*>* getNodess();
	private:
		
};

#endif
