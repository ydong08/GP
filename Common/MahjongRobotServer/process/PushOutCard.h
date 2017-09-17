#ifndef _PushOutCard_H_
#define _PushOutCard_H_

#include "BaseProcess.h"

class PushOutCard :public BaseProcess
{
	public:
		PushOutCard();
		virtual ~PushOutCard();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

