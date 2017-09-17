#ifndef _PhpReChargeProc_H_
#define _PhpReChargeProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class PhpReChargeProc :public IProcess
{
	public:
		PhpReChargeProc();
		virtual ~PhpReChargeProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

