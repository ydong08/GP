#ifndef _CreatePrivateProc_H_
#define _CreatePrivateProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class CreatePrivateProc :public IProcess
{
	public:
		CreatePrivateProc();
		virtual ~CreatePrivateProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

