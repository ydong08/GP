#ifndef _GetIdleTableProc_H_
#define _GetIdleTableProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class GetIdleTableProc  : public BaseProcess
{
public:
	GetIdleTableProc(){};
	virtual ~GetIdleTableProc(){};
	 
	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendErrorMsg(CDLSocketHandler* clientHandler, short errcode, const char* errmsg);
};
#endif



