#ifndef _KickOutPlayerProc_H_
#define _KickOutPlayerProc_H_

#include "BaseProcess.h"

class KickOutPlayerProc :public BaseProcess
{
	public:
		KickOutPlayerProc();
		virtual ~KickOutPlayerProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

