#ifndef _UserLogoutProc_H_
#define _UserLogoutProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class UserLogoutProc :public IProcess
{
	public:
		UserLogoutProc();
		virtual ~UserLogoutProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

