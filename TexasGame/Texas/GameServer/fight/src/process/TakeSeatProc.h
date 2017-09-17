#ifndef _TakeSeatProc_H_
#define _TakeSeatProc_H_

#include "BaseProcess.h"

class TakeSeatProc :public BaseProcess
{
	public:
		TakeSeatProc();
		virtual ~TakeSeatProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
	private:
};

#endif

