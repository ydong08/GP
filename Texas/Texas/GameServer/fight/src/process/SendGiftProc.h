#ifndef _SendGiftProc_H_
#define _SendGiftProc_H_

#include "BaseProcess.h"

class SendGiftProc :public BaseProcess
{
	public:
		SendGiftProc();
		virtual ~SendGiftProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt );
};

#endif

