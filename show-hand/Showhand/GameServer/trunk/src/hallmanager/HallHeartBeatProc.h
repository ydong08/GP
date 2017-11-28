#ifndef _HallHeartBeatProc_H_
#define _HallHeartBeatProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class HallHeartBeatProc :public IProcess
{
	public:
		HallHeartBeatProc();
		virtual ~HallHeartBeatProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

