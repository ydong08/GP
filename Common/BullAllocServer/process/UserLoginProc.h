#ifndef _UserLoginProc_H_
#define _UserLoginProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class UserLoginProc :public IProcess
{
	public:
		UserLoginProc();
		virtual ~UserLoginProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

