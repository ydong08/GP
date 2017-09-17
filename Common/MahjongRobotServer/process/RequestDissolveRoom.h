#ifndef __RequestDissolveRoom_H__
#define __RequestDissolveRoom_H__

#include "BaseProcess.h"

class RequestDissolveRoom : public BaseProcess
{
public:
	RequestDissolveRoom();
	~RequestDissolveRoom();

	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif // !__RequestDissolveRoom_H__
