#ifndef _USTrumptProc_H_
#define _USTrumptProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class USTrumptProc :public IProcess
{
	public:
		USTrumptProc();
		virtual ~USTrumptProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

