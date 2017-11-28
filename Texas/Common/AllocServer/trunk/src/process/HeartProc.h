#ifndef _HeartProc_H_
#define _HeartProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class HeartProc :public IProcess
{
	public:
		HeartProc();
		virtual ~HeartProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

