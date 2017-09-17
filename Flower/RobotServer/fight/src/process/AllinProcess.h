#ifndef _AllinProcess_H_
#define _AllinProcess_H_

#include "BaseProcess.h"

class AllinProcess :public BaseProcess
{
	public:
		AllinProcess();
		virtual ~AllinProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

