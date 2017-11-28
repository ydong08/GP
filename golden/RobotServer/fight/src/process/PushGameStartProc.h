#ifndef _PushGameStartProc_H_
#define _PushGameStartProc_H_

#include "BaseProcess.h"

class PushGameStartProc :public BaseProcess
{
	public:
		PushGameStartProc();
		virtual ~PushGameStartProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

