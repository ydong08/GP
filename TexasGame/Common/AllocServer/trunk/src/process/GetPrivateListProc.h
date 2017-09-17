#ifndef _GetPrivateListProc_H_
#define _GetPrivateListProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class GetPrivateListProc :public IProcess
{
	public:
		GetPrivateListProc();
		virtual ~GetPrivateListProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

