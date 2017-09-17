#ifndef _PushForbidProc_H_
#define _PushForbidProc_H_

#include "BaseProcess.h"

class PushForbidProc :public BaseProcess
{
	public:
		PushForbidProc();
		virtual ~PushForbidProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

