#ifndef _TrumptProc_H_
#define _TrumptProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class TrumptProc :public IProcess
{
	public:
		TrumptProc();
		virtual ~TrumptProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

