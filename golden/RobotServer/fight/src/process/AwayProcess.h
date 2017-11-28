#ifndef _AwayProcess_H_
#define _AwayProcess_H_

#include "BaseProcess.h"

class AwayProcess :public BaseProcess
{
	public:
		AwayProcess();
		virtual ~AwayProcess();

		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

