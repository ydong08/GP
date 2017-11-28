#ifndef _GetPlayInfo_H_
#define _GetPlayInfo_H_

#include "BaseProcess.h"

class GetPlayInfo :public BaseProcess
{
	public:
		GetPlayInfo();
		virtual ~GetPlayInfo();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	public:
		int productBankerRobot(int tid);
		int productBetRobot(int tid);
};

#endif

