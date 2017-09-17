#ifndef _AllocProcess_H_
#define _AllocProcess_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class AllocProcess :public IProcess
{
	public:
		AllocProcess();
		virtual ~AllocProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

