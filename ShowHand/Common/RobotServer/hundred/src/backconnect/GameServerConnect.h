#ifndef __GameServerConnect_H__
#define __GameServerConnect_H__

#include <map>
#include "CDL_Timer_Handler.h"
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"

class GameConnector;

class GameConnectHandler :public CDLSocketHandler 
{
	public:
		GameConnectHandler(GameConnector* connector);
		virtual ~GameConnectHandler();
        int OnConnected() ;
        int OnClose();
        int OnPacketComplete(const char* data, int len);
		
		CDL_Decoder* CreateDecoder()
    	{
			return &DLDecoder::getInstance();
    	}

	private:
		GameConnector * connector;
		
};

class GameConnector: public CCTimer 
{
	public:
		
		GameConnector(int svid);
		virtual ~GameConnector();
		int connect(const char* ip, short port);
		int reconnect();
		inline bool isActive(){return this->isConnected;};
		inline bool isSuspended(){return this->suspended;};
		inline void setSuspended(bool b){  this->suspended = b;};
		int send(OutputPacket* outPack, bool isEncrypt=true);
		int send(InputPacket* inPack);
		int send(char* buff, int len);
		void ScanningServer();

	private:
		virtual int ProcessOnTimerOut();
		GameConnectHandler* handler;
		bool isConnected;
		bool suspended;
		char ip[128];
		short port;
		int svid;
	friend class GameConnectHandler;
 
};

class BackGameConnectManager
{
	public:
		static void addGameNode(int svid, GameConnector* backgamenode);
		static GameConnector* getGameNode(int svid);
		static void delGameNode(int svid);

	private:
		
};

#endif
