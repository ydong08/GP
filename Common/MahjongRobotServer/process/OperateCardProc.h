
#pragma once


#include "BaseProcess.h"

class OperateCardProc :public BaseProcess
{
public:
	OperateCardProc();
	virtual ~OperateCardProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
private:
};