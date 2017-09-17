#ifndef _TimeOutProcess_H_
#define _TimeOutProcess_H_

#include "BaseProcess.h"

class TimeOutProcess :public BaseProcess
{
	public:
		TimeOutProcess();
		virtual ~TimeOutProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

