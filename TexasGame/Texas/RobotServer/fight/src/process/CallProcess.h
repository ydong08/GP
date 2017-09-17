#ifndef _CallProcess_H_
#define _CallProcess_H_

#include "IProcess.h"

class CallProcess :public IProcess
{
	public:
		CallProcess();
		virtual ~CallProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

