#ifndef _PushCardProc_H_
#define _PushCardProc_H_

#include "BaseProcess.h"

class PushCardProc :public BaseProcess
{
public:
	PushCardProc();
	virtual ~PushCardProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif
