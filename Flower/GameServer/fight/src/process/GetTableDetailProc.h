#ifndef _GetTableDetailProc_H_
#define _GetTableDetailProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class GetTableDetailProc :public BaseProcess
{
public:
	GetTableDetailProc();
	virtual ~GetTableDetailProc();
		
	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
};

#endif

