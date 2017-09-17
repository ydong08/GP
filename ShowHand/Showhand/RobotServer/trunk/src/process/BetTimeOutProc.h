#ifndef _BetTimeOutProc_H_
#define _BetTimeOutProc_H_

#include "IProcess.h"

class BetTimeOutProc :public IProcess
{
	public:
		BetTimeOutProc();
		virtual ~BetTimeOutProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

