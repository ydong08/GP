#ifndef __TransCreateRoomProc_H__
#define __TransCreateRoomProc_H__

#include "IProcess.h"

class TransCreateRoomProc : public IProcess
{
public:
	TransCreateRoomProc();
	virtual ~TransCreateRoomProc();

	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif // !__TransCreateRoomProc_H__