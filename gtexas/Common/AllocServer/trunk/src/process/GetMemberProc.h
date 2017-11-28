#ifndef _GetMemberProc_H_
#define _GetMemberProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class GetMemberProc :public IProcess
{
	public:
		GetMemberProc();
		virtual ~GetMemberProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

