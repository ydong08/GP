#ifndef _GetFreePlayer_H_
#define _GetFreePlayer_H_

#include "BaseProcess.h"

class BaseClientHandler;

class GetFreePlayer : public BaseProcess
{
public:
	GetFreePlayer() {};
	virtual ~GetFreePlayer() {};

	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

};

#endif
