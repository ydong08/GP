
#ifndef RoundServerConnect_H
#define RoundServerConnect_H
#include "CDLSocketHandler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
#include "DLDecoder.h"

#define CMD_BACKSVR_HEART	            0x0001     // ÐÄÌø°ü

class RoundServerConnect ;

class RoundHeartBeatTimer:public CCTimer
{
	public:
		RoundHeartBeatTimer(){} ;
		virtual ~RoundHeartBeatTimer() {};

		inline void init(RoundServerConnect* backsrv){this->backsrv=backsrv;};
	private:
		virtual int ProcessOnTimerOut();

		RoundServerConnect* backsrv;
	 		
};

class RoundSocketHandle: public CDLSocketHandler 
{
	public:
		RoundSocketHandle(RoundServerConnect* server);
		RoundSocketHandle(){};
		virtual ~RoundSocketHandle(){
			//this->_decode = NULL;
		};

		int OnConnected();
		int OnClose();
		int OnPacketComplete(const char * data, int len)
        {
            pPacket.Copy(data,len);
            return OnPacketComplete(&pPacket);
        }
		int OnPacketComplete(InputPacket* pPacket);
		CDL_Decoder* CreateDecoder()
        {
			return &DLDecoder::getInstance();
        }

	private :
		RoundServerConnect* server;
		InputPacket pPacket;
};
 

class RoundServerConnect 
{
	public:
		static RoundServerConnect* getInstance();
		virtual ~RoundServerConnect();
		int connect(const char* ip, short port);
		int reconnect();
		inline bool isActive(){return isConnect;};
		inline void setActive(bool b){ this->isConnect = b;  if(!b) handler=NULL; };
		
		int Send(InputPacket* inputPacket)
		{
			if(handler)
				return handler->Send(inputPacket->packet_buf(), inputPacket->packet_size()) >=0 ? 0 : -1;
			else
			{
				if(reconnect()==0)
				{
					return handler->Send(inputPacket->packet_buf(), inputPacket->packet_size())  >=0 ? 0 : -1;
				}
				return -1;
			}
		}

		int Send(OutputPacket* outputPacket)
		{
			if(handler)
				return handler->Send(outputPacket->packet_buf(), outputPacket->packet_size())  >=0 ? 0 : -1;
			else
			{
				if(reconnect()==0)
				{
					return handler->Send(outputPacket->packet_buf(), outputPacket->packet_size())  >=0 ? 0 : -1;
				}
				return -1;
			}
		}

		int sendHeartBeat();
	
	private:
		RoundServerConnect();
		short port;
		char ip[32];
		bool isConnect;

		RoundSocketHandle* handler;
		RoundHeartBeatTimer* heartBeatTimer;
};

 


#endif
