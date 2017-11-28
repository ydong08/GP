#ifndef _ComingGameProc_H_
#define _ComingGameProc_H_

#include "IProcess.h"

class ComingGameProc :public IProcess
{
	public:
		ComingGameProc();
		virtual ~ComingGameProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

