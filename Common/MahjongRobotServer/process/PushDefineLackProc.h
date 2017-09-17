#ifndef _PushDefineLackProc_H_
#define _PushDefineLackProc_H_

#include "BaseProcess.h"

class PushDefineLackProc :public BaseProcess
{
	public:
		PushDefineLackProc();
		virtual ~PushDefineLackProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

