#ifndef _StartRobotProc_H_
#define _StartRobotProc_H_

#include "BaseProcess.h"

class StartRobotProc :public BaseProcess
{
	public:
		StartRobotProc();
		virtual ~StartRobotProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt );
};

#endif

