#ifndef _LevelMatchProc_H_
#define _LevelMatchProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class LevelMatchProc :public IProcess
{
	public:
		LevelMatchProc();
		virtual ~LevelMatchProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

