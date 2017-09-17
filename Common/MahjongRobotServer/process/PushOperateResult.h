#ifndef _PushOutCard_H_
#define _PushOutCard_H_

#include "BaseProcess.h"

class PushOperateResult :public BaseProcess
{
	public:
		PushOperateResult();
		virtual ~PushOperateResult();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

