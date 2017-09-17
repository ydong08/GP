#ifndef _PushChiHuPro_H_
#define _PushChiHuPro_H_

#include "BaseProcess.h"

class PushChiHuPro :public BaseProcess
{
	public:
		PushChiHuPro();
		virtual ~PushChiHuPro();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

