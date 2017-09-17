#ifndef _LoginHallProc_H_
#define _LoginHallProc_H_

#include "BaseProcess.h"

class LoginHallProc :public BaseProcess
{
	public:
		LoginHallProc();
		virtual ~LoginHallProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

