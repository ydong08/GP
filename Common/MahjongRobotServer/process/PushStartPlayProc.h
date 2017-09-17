#ifndef _PushStartPlayProc_H_
#define _PushStartPlayProc_H_

#include "BaseProcess.h"

class PushStartPlayProc :public BaseProcess
{
	public:
		PushStartPlayProc();
		virtual ~PushStartPlayProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

