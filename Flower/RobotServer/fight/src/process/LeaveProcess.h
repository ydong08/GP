#ifndef _LeaveProcess_H_
#define _LeaveProcess_H_

#include "BaseProcess.h"

class LeaveProcess :public BaseProcess
{
	public:
		LeaveProcess();
		virtual ~LeaveProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

