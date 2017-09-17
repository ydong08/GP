
#ifndef TrumptConnect_H
#define TrumptConnect_H
#include "CDLSocketHandler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
#include "DLDecoder.h"

#define CMD_BACKSVR_HEART	            0x0001     // 心跳包

class TrumptConnect ;

class TrumptHeartBeatTimer:public CCTimer
{
	public:
		TrumptHeartBeatTimer(){} ;
		virtual ~TrumptHeartBeatTimer() {};

		inline void init(TrumptConnect* backsrv){this->backsrv=backsrv;};
	private:
		virtual int ProcessOnTimerOut();

		TrumptConnect* backsrv;
	 		
};

class TrumptSocketHandle: public CDLSocketHandler 
{
	public:
		TrumptSocketHandle(TrumptConnect* server);
		TrumptSocketHandle(){};
		virtual ~TrumptSocketHandle(){
		//	this->_decode = NULL;
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
				//return new DLDecoder();
        }

	private :
		TrumptConnect* server;
		InputPacket pPacket;
};
 

class TrumptConnect 
{
	public:
		static TrumptConnect* getInstance();
		virtual ~TrumptConnect();
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
		TrumptConnect();
		short port;
		char ip[32];
		bool isConnect;

		TrumptSocketHandle* handler;
		TrumptHeartBeatTimer* heartBeatTimer;
};

 


#endif
