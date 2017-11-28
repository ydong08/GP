#ifndef _HeartProcess_H_
#define _HeartProcess_H_

#include "BaseProcess.h"

class HeartProcess :public BaseProcess
{
	public:
		HeartProcess();
		virtual ~HeartProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

