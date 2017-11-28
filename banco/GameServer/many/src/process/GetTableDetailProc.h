#ifndef _GetTableDetailProc_H_
#define _GetTableDetailProc_H_

#include "BaseProcess.h"

class GetTableDetailProc :public BaseProcess
{
	public:
		GetTableDetailProc();
		virtual ~GetTableDetailProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;

};

#endif

