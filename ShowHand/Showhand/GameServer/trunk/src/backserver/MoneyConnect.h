#ifndef __MoneyConnect_H__
#define __MoneyConnect_H__

#include <map> 
#include "CDL_Timer_Handler.h"
#include "CDLSocketHandler.h"
#include "DLDecoder.h"
#include "SvrConfig.h"

#define CMD_BACKSVR_HEART                   0x0001

const int MAX_BACK_CONN = 128;
class MoneyNodes;
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
		char ip[128];
		short port;
		int id;
		int fd;

	friend class ConnectHandler;
	friend class MoneyNodes;
 
};

class MoneyNodes: public CCTimer 
{
        public:
				MoneyNodes(int id);
                virtual ~MoneyNodes();
				int addNode(Node* node);
				int connect(int id, char* host, int port);
				int send(int id,OutputPacket* outPack, bool isEncrypt=true); 
				int send(OutputPacket* outPack, bool isEncrypt=true); 
				int send(int id,InputPacket* inputPack); 
				int send(InputPacket* inputPack);
				int sendRandnode(InputPacket* inputPack);
				int sendRandnode(OutputPacket* outPack, bool isEncrypt=true);
		private:
				virtual int ProcessOnTimerOut();
				Connector* connectors[MAX_BACK_CONN];
				int conn_size; 
				int id;
};

class MoneyConnectManager
{
	public:
		static void addNodes(int id, MoneyNodes* backnodes);
		static MoneyNodes* getNodes(int id);
	private:
		
};

#endif
