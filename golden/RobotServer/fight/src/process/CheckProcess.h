#ifndef _CheckProcess_H_
#define _CheckProcess_H_

#include "BaseProcess.h"

class CheckProcess :public BaseProcess
{
	public:
		CheckProcess();
		virtual ~CheckProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

