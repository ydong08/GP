#ifndef _GetIdleTableProc_H_
#define _GetIdleTableProc_H_

#include "IProcess.h"

class GetIdleTableProc :public IProcess
{
	public:
		GetIdleTableProc();
		virtual ~GetIdleTableProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
	public:
		int productRobot(int tid, short status, short level, short countUser);
};

#endif

