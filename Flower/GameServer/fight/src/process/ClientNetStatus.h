#ifndef _CLIENT_NET_STATUS_PROCESS_H_
#define _CLIENT_NET_STATUS_PROCESS_H_

#include "BaseProcess.h"

class ClientNetStatusProcess :public BaseProcess
{
public:
    ClientNetStatusProcess();
    virtual ~ClientNetStatusProcess();
		
	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif

