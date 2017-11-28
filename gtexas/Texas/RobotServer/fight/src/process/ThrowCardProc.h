#ifndef _ThrowCardProc_H_
#define _ThrowCardProc_H_

#include "IProcess.h"

class ThrowCardProc :public IProcess
{
	public:
		ThrowCardProc();
		virtual ~ThrowCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

