#ifndef _DefienLackProc_H_
#define _DefienLackProc_H_

#include "BaseProcess.h"

class DefineLackProc :public BaseProcess
{
public:
	DefineLackProc();
	virtual ~DefineLackProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
private:
};

#endif