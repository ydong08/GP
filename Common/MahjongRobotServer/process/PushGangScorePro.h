#ifndef _PushGangScorePro_H_
#define _PushGangScorePro_H_

#include "BaseProcess.h"

class PushGangScorePro :public BaseProcess
{
	public:
		PushGangScorePro();
		virtual ~PushGangScorePro();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

