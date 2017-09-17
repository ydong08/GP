#ifndef _CompareCardProc_H_
#define _CompareCardProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class CompareCardProc :public BaseProcess
{
public:
	CompareCardProc();
	virtual ~CompareCardProc();
		
	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

};

#endif

