#ifndef _LookCardProc_H_
#define _LookCardProc_H_

#include "IProcess.h"

class LookCardProc :public IProcess
{
	public:
		LookCardProc();
		virtual ~LookCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

