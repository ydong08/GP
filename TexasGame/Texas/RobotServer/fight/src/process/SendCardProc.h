#ifndef _SendCardProc_H_
#define _SendCardProc_H_

#include "IProcess.h"

class SendCardProc :public IProcess
{
	public:
		SendCardProc();
		virtual ~SendCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

