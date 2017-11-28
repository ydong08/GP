#ifndef _GetBankerProc_H_
#define _GetBankerProc_H_

#include "BaseProcess.h"

class GetBankerProc :public BaseProcess
{
public:
	GetBankerProc();
	virtual ~GetBankerProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif

