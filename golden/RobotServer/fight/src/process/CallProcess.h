#ifndef _CallProcess_H_
#define _CallProcess_H_

#include "BaseProcess.h"

class CallProcess :public BaseProcess
{
	public:
		CallProcess();
		virtual ~CallProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

