#ifndef _ChatProcess_H_
#define _ChatProcess_H_

#include "BaseProcess.h"

class ChatProcess :public BaseProcess
{
	public:
		ChatProcess();
		virtual ~ChatProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt );
};

#endif

