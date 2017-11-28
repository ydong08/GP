#ifndef _GetAllUserStatus_H_
#define _GetAllUserStatus_H_

#include "BaseProcess.h"

class GetAllUserStatus : public BaseProcess
{
public:
	GetAllUserStatus() {};
	virtual ~GetAllUserStatus() {};

	virtual int doRequest(CDLSocketHandler* CDLSocketHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* CDLSocketHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendErrorMsg(CDLSocketHandler* CDLSocketHandler, short errcode, char* errmsg);
};

#endif
