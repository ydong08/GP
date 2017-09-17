#ifndef _LeaveProcess_H_
#define _LeaveProcess_H_

#include "IProcess.h"

class LeaveProcess :public IProcess
{
	public:
		LeaveProcess();
		virtual ~LeaveProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

