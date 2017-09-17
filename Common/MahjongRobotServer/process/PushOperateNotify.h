#ifndef _PushOperateNotify_H_
#define _PushOperateNotify_H_

#include "BaseProcess.h"

class PushOperateNotify :public BaseProcess
{
	public:
		PushOperateNotify();
		virtual ~PushOperateNotify();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

