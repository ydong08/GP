#ifndef _ForbidProc_H_
#define _ForbidProc_H_

#include "BaseProcess.h"

class ForbidProc :public BaseProcess
{
	public:
		ForbidProc();
		virtual ~ForbidProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

