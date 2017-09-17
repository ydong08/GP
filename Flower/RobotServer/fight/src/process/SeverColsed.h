#ifndef _SeverColsed_H_
#define _SeverColsed_H_

#include "BaseProcess.h"

class SeverColsed :public BaseProcess
{
	public:
		SeverColsed();
		virtual ~SeverColsed();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

