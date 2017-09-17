#ifndef _OperateProc_H_
#define _OperateProc_H_

#include "BaseProcess.h"

class OperateProc :public BaseProcess
{
	public:
		OperateProc();
		virtual ~OperateProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

