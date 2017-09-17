#ifndef _PushChangeCardProc_H_
#define _PushChangeCardProc_H_

#include "BaseProcess.h"

class PushChangeCardProc :public BaseProcess
{
	public:
		PushChangeCardProc();
		virtual ~PushChangeCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

