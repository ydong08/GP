#ifndef _GetPlayerListProc_H_
#define _GetPlayerListProc_H_

#include "BaseProcess.h"

class GetPlayerListProc :public BaseProcess
{
public:
	GetPlayerListProc();
	virtual ~GetPlayerListProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif
