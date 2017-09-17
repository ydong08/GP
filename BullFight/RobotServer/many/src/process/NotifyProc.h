

#pragma once

#include "IProcess.h"

class NotifyProc :public IProcess
{
public:
	NotifyProc();
	virtual ~NotifyProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};