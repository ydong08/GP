#ifndef  _CDL_UDP_SERVER_H_
#define  _CDL_UDP_SERVER_H_
#include <string.h>
#include "poller.h"
#include "net.h"

class CDL_UDP_Server : public CPollerObject 
{
	public:
		CDL_UDP_Server(){};
		virtual ~CDL_UDP_Server(){};

		virtual int OnPacketComplete(const char * data, int len,char* ip,int port)=0;
		virtual int ParsePacket(char * data, int len)=0;
		virtual int Send(const char * buff, int len,char* dst_ip,int dst_port)=0;
};



#endif


