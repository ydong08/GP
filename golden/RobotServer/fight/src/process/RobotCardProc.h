#ifndef _RobotCardProc_H_
#define _RobotCardProc_H_

#include "BaseProcess.h"

class RobotCardProc :public BaseProcess
{
	public:
		RobotCardProc();
		virtual ~RobotCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};
#endif

