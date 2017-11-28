#ifndef BASEALLOC_CONNECT_H_
#define BASEALLOC_CONNECT_H_
#include "CDL_Timer_Handler.h"
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"


class BaseAllocConnect;

class ReportTimer:public CCTimer
{
	public:
		ReportTimer(){} ;
		virtual ~ReportTimer() {};

        inline void init(BaseAllocConnect* allocsvr){this->allocsvr=allocsvr;};
private:
		virtual int ProcessOnTimerOut();

        BaseAllocConnect* allocsvr;
	 		
};


class SocketHandle: public CDLSocketHandler 
{
	public:
        SocketHandle(BaseAllocConnect* allocsvr);
		SocketHandle(){};
		virtual ~SocketHandle(){
		//	this->_decode = NULL;
		};

		int OnConnected();
		int OnClose();
        int OnPacketComplete(const char * data, int len);

		CDL_Decoder* CreateDecoder()
		{
			return &DLDecoder::getInstance();
		}
		int Send(OutputPacket* packet, bool encode=true)
		{
			if(encode)
				packet->EncryptBuffer();
			return CDLSocketHandler::Send(packet->packet_buf(), packet->packet_size()) >= 0 ? 0 : -1;
		}

	private :
        BaseAllocConnect* allocsvr;
		InputPacket pPacket;
};
 
class BaseAllocConnect
{
    public:
        virtual ~BaseAllocConnect();
		int connect(const char* ip, short port);
		int reconnect();
        inline bool isActive(){return isConnect;}
        inline void setActive(bool b){ this->isConnect = b;  if(!b) handler=NULL; }

        virtual int reportSeverInfo() { return 0;} //上报服务器Info
        virtual int updateSeverInfo() { return 0;} //更新服务器Info：在线人数

		int trumptToUser(short type, const char* msg, short pid);

        virtual int processReportResponse(InputPacket* pPacket) { return 0;}
        virtual int processUpdateResponse(InputPacket* pPacket) { return 0;}
        virtual int processSetServerStatus(InputPacket* pPacket) { return 0;}
        virtual int processOtherMsg(InputPacket* pPacket) { return 0;}

    protected:
        BaseAllocConnect();

		short port;
		char ip[32];
		bool isConnect;
		ReportTimer* reportTimer;
		SocketHandle* handler;
		
		int Send(OutputPacket* ReportPkg, bool encode=true)
		{
			if(handler)
				return handler->Send(ReportPkg,encode);
			else
				return -1;
		}
};

 


#endif
