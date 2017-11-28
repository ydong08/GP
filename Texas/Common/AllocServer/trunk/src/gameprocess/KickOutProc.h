#ifndef _KickOutProc_H_
#define _KickOutProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class KickOutProc :public IProcess
{
	public:
		KickOutProc();
		virtual ~KickOutProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

