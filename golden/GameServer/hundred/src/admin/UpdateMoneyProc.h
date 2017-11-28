#ifndef _UpdateMoneyProc_H_
#define _UpdateMoneyProc_H_

#include "BaseProcess.h"

class BaseClientHandler;

class UpdateMoneyProc : public BaseProcess
{
public:
	UpdateMoneyProc() {};
	virtual ~UpdateMoneyProc() {};

	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif
