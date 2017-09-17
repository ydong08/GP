#ifndef _BASE_PROCESS_H_
#define _BASE_PROCESS_H_

#include "BaseProcess.h"

class CompareProcess :public BaseProcess
{
	public:
		CompareProcess();
		virtual ~CompareProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

