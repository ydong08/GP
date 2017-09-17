
#pragma once


#include "BaseProcess.h"

class OutCardProc :public BaseProcess
{
public:
	OutCardProc();
	virtual ~OutCardProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
private:
};