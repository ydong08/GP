
#ifndef LogServerConnect_H
#define LogServerConnect_H
#include "CDLSocketHandler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
#include "DLDecoder.h"
#define CMD_BACKSVR_HEART	            0x0001     //MysqlServer

#define RECORD_GAME_START			0x0500	//游戏开始
#define RECORD_BET_CALL				0x0501	//跟注
#define RECORD_BET_RASE				0x0502	//加注
#define RECORD_ALL_IN				0x0503	//梭哈
#define RECORD_LOOK_CARD			0x0504	//看牌
#define RECORD_THROW_CARD			0x0505	//弃牌
#define RECORD_GAME_OVER			0x0506	//游戏结束 
#define RECORD_ACTIVE_LEAVE			0x0507	//游戏中离开 
#define RECORD_LOG_OUT				0x0508	//用户断线 
#define RECORD_GET_DETAIL			0x0509	//获取信息 
#define RECORD_TIME_OUT				0x0510	//获取信息 

class LogServerConnect ;

class HeartTimer:public CCTimer
{
	public:
		HeartTimer(){} ;
		virtual ~HeartTimer() {};

		inline void init(LogServerConnect* connect){this->connect=connect;};
private:
		virtual int ProcessOnTimerOut();

		LogServerConnect* connect;
	 		
};

class ConnectHandle: public CDLSocketHandler 
{
	public:
		ConnectHandle(LogServerConnect* connect);
		ConnectHandle(){};
		virtual ~ConnectHandle()
		{
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
		LogServerConnect* connect;
		InputPacket pPacket;
};
 

class LogServerConnect 
{
	public:
		static LogServerConnect* getInstance();
		virtual ~LogServerConnect();
		int connect(const char* ip, short port);
		int reconnect();
		inline bool isActive(){return isConnect;};
		inline void setActive(bool b){ this->isConnect = b;  if(!b) handler=NULL; };
		int reportLog(int time, int svid, int uid, int opt,const char* format,  ...);
		
		int Send(InputPacket* inputPacket)
		{
			if(handler)
				return handler->Send(inputPacket->packet_buf(), inputPacket->packet_size()) >=0 ? 0 : -1;
			else{
				/*if(reconnect()==0)
				{
					return handler->Send(inputPacket->packet_buf(), inputPacket->packet_size());
				}*/
				return -1;
			}
		}

		int Send(OutputPacket* outputPacket)
		{
			if(handler)
				return handler->Send(outputPacket->packet_buf(), outputPacket->packet_size())  >=0 ? 0 : -1;
			else{
				/*if(reconnect()==0)
				{
					return handler->Send(outputPacket->packet_buf(), outputPacket->packet_size());
				}*/
				return -1;
			}
		}

		int sendHeartBeat();
	
	private:
		LogServerConnect();
		short port;
		char ip[32];
		bool isConnect;

		ConnectHandle* handler;
		HeartTimer* heartBeatTimer;
};

 


#endif
