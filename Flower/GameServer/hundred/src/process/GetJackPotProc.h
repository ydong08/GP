#ifndef _GetJackPotProc_H_
#define _GetJackPotProc_H_

#include "BaseProcess.h"

class GetJackPotProc :public BaseProcess
{
public:
	GetJackPotProc();
	virtual ~GetJackPotProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif
