#ifndef _PushSendCardProc_H_
#define _PushSendCardProc_H_

#include "BaseProcess.h"

class PushSendCardProc :public BaseProcess
{
	public:
		PushSendCardProc();
		virtual ~PushSendCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

