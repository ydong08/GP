#ifndef _LoginHallProc_H_
#define _LoginHallProc_H_

#include "IProcess.h"

class LoginHallProc :public IProcess
{
	public:
		LoginHallProc();
		virtual ~LoginHallProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

