#ifndef _PushGameStartProc_H_
#define _PushGameStartProc_H_

#include "IProcess.h"

class PushGameStartProc :public IProcess
{
	public:
		PushGameStartProc();
		virtual ~PushGameStartProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

