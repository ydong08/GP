#ifndef _QuickMatchProc_H_
#define _QuickMatchProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class QuickMatchProc :public IProcess
{
	public:
		QuickMatchProc();
		virtual ~QuickMatchProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

