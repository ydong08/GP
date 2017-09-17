#ifndef _ComingGameProc_H_
#define _ComingGameProc_H_

#include "BaseProcess.h"

class ComingGameProc :public BaseProcess
{
	public:
		ComingGameProc();
		virtual ~ComingGameProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

