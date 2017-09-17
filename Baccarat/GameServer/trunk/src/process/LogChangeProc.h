#ifndef _LogChangeProc_H_
#define _LogChangeProc_H_

#include "BaseProcess.h"

class LogChangeProc : public BaseProcess
{
	public:
		LogChangeProc();
		virtual ~LogChangeProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt);
};

#endif

