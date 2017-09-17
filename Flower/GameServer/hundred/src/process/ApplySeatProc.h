#ifndef _ApplySeatProc_H_
#define _ApplySeatProc_H_

#include "BaseProcess.h"

class ApplySeatProc :public BaseProcess
{
public:
	ApplySeatProc();
	virtual ~ApplySeatProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif
