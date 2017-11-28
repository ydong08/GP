#ifndef _RaseProcess_H_
#define _RaseProcess_H_

#include "IProcess.h"

class RaseProcess :public IProcess
{
	public:
		RaseProcess();
		virtual ~RaseProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

