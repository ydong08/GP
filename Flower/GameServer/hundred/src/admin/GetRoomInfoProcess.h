#ifndef _GetRoomInfoProcess_H_
#define _GetRoomInfoProcess_H_

#include "BaseProcess.h"

class GetRoomInfoProcess : public BaseProcess
{
public:
	GetRoomInfoProcess() {};
	virtual ~GetRoomInfoProcess() {};

	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int doGetRoomInfo(CDLSocketHandler* clientHandler, int PageNo, int PageNum);
};

#endif
