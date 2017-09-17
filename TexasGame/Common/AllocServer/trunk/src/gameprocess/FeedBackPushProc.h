#ifndef _FeedBackPushProc_H_
#define _FeedBackPushProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class FeedBackPushProc :public IProcess
{
	public:
		FeedBackPushProc();
		virtual ~FeedBackPushProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

