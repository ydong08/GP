#ifndef _ChangeTabProc_H_
#define _ChangeTabProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class ChangeTabProc :public IProcess
{
	public:
		ChangeTabProc();
		virtual ~ChangeTabProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

