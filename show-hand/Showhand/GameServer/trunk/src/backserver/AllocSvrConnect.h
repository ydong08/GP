#ifndef AllocSvrConnect_H
#define AllocSvrConnect_H
#include "CDL_Timer_Handler.h"
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"
#include "Room.h"


class AllocSvrConnect;

class ReportTimer:public CCTimer
{
	public:
		ReportTimer(){} ;
		virtual ~ReportTimer() {};

		inline void init(AllocSvrConnect* allocsvr){this->allocsvr=allocsvr;};
private:
		virtual int ProcessOnTimerOut();

		AllocSvrConnect* allocsvr;	
};


class SocketHandle: public CDLSocketHandler 
{
	public:
		SocketHandle(AllocSvrConnect* allocsvr);
		SocketHandle(){};
		virtual ~SocketHandle(){
			//this->_decode = NULL;
		};

		int OnConnected();
		int OnClose();
		int OnPacketComplete(const char * data, int len)
		{
			pPacket.Copy(data,len);
			return OnPacketComplete( &pPacket);
		}
		int OnPacketComplete(InputPacket* pPacket);

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
		AllocSvrConnect* allocsvr;
		InputPacket pPacket;
};
 

class AllocSvrConnect 
{
	public:
		static AllocSvrConnect* getInstance();
		virtual ~AllocSvrConnect();
		int connect(const char* ip, short port);
		int reconnect();
		inline bool isActive(){return isConnect;};
		inline void setActive(bool b){ this->isConnect = b;  if(!b) handler=NULL; };

		int reportSeverInfo();
		int updateSeverInfo();
		int setSeverStatus();
		int getSeverInfo();
		int userLeaveGame(Player* player, int uid = 0);
		int userEnterGame(Player* player);
        int userUpdateStatus(Player* player, short nstatus);
		int updateTableUserCount(Table* table);
		int updateTableStatus(Table* table);

		int trumptComTasToUser(short type, const char* msg, short pid);

		int processReportResponse(InputPacket* pPacket);
		int processUpdateResponse(InputPacket* pPacket);
		int processSetServerStatus(InputPacket* pPacket);
		int processSeverInfoResponse(InputPacket* pPacket);
		int processCreatePrivateTable(InputPacket* pPacket);
		


	private:
		AllocSvrConnect();

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
