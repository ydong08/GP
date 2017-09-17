#ifndef _PushTrusteePro_H_
#define _PushTrusteePro_H_

#include "BaseProcess.h"

class PushTrusteePro :public BaseProcess
{
	public:
		PushTrusteePro();
		virtual ~PushTrusteePro();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

