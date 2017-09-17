#ifndef _PushDispatchCardProc_H_
#define _PushDispathCard_H_

#include "BaseProcess.h"

class PushDispatchCardProc :public BaseProcess
{
	public:
		PushDispatchCardProc();
		virtual ~PushDispatchCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

