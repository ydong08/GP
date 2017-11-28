#ifndef _HeartProcess_H_
#define _HeartProcess_H_

#include "IProcess.h"

class HeartProcess :public IProcess
{
	public:
		HeartProcess();
		virtual ~HeartProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

