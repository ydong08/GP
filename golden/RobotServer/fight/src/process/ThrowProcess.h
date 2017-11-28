#ifndef _ThrowProcess_H_
#define _ThrowProcess_H_

#include "BaseProcess.h"

class ThrowProcess :public BaseProcess
{
	public:
		ThrowProcess();
		virtual ~ThrowProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

