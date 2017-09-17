#ifndef _ChangeCardResultProc_H_
#define _ChangeCardResultProc_H_

#include "BaseProcess.h"

class ChangeCardResultProc :public BaseProcess
{
public:
	ChangeCardResultProc();
	virtual ~ChangeCardResultProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
private:
};

#endif