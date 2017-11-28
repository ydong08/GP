#ifndef _RobotCardProc_H_
#define _RobotCardProc_H_

#include "IProcess.h"

class RobotCardProc :public IProcess
{
	public:
		RobotCardProc();
		virtual ~RobotCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

