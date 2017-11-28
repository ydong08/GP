#ifndef _HallRegistProc_H_
#define _HallRegistProc_H_

#include "IProcess.h"

class HallRegistProc :public IProcess
{
	public:
		HallRegistProc();
		virtual ~HallRegistProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt); 
};

#endif

