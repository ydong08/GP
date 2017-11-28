#ifndef _RASE_PROCESS_H_
#define _RASE_PROCESS_H_

#include "BaseProcess.h"

class RaseProcess :public BaseProcess
{
	public:
		RaseProcess();
		virtual ~RaseProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ); 
};

#endif

