
#ifndef MySqlConnect_H
#define MySqlConnect_H
#include "CDL_TCP_Handler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
#include "DLDecoder.h"

#define CMD_BACKSVR_HEART	            0x0001     //MysqlServerÐÄÌø°ü

class MySqlConnect ;

class HeartBeatTimer:public CCTimer
{
	public:
		HeartBeatTimer(){} ;
		virtual ~HeartBeatTimer() {};

		inline void init(MySqlConnect* backsrv){this->backsrv=backsrv;};
	private:
		virtual int ProcessOnTimerOut();

		MySqlConnect* backsrv;
	 		
};

class MySqlHandle: public CDLSocketHandler 
{
	public:
		MySqlHandle(MySqlConnect* server);
		MySqlHandle(){};
		virtual ~MySqlHandle(){
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
		MySqlConnect* server;
		InputPacket pPacket;
};
 

class MySqlConnect 
{
	public:
		static MySqlConnect* getInstance();
		virtual ~MySqlConnect();
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
		MySqlConnect();
		short port;
		char ip[32];
		bool isConnect;

		MySqlHandle* handler;
		HeartBeatTimer* heartBeatTimer;
};

 


#endif
