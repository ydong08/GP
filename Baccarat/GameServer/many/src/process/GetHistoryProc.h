#ifndef _GetHistoryProc_H_
#define _GetHistoryProc_H_

#include "BaseProcess.h"

class GetHistoryProc :public BaseProcess
{
	public:
		GetHistoryProc();
		virtual ~GetHistoryProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;

};

#endif

