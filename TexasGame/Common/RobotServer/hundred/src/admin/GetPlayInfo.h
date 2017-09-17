#ifndef _GetPlayInfo_H_
#define _GetPlayInfo_H_

#include "IProcess.h"

class GetPlayInfo :public IProcess
{
	public:
		GetPlayInfo();
		virtual ~GetPlayInfo();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
	public:
		int productBankerRobot(int tid);
		int productBetRobot(int tid);
};

#endif

