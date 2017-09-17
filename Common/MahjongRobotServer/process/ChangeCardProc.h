#ifndef _ChangeCardProc_H_
#define _ChangeCardProc_H_

#include "BaseProcess.h"

class ChangeCardProc :public BaseProcess
{
public:
	ChangeCardProc();
	virtual ~ChangeCardProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
private:
};

#endif