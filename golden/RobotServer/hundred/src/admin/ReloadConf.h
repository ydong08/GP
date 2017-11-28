#ifndef _ReloadConf_H_
#define _ReloadConf_H_

#include "BaseProcess.h"

class ReloadConf :public BaseProcess
{
	public:
		ReloadConf();
		virtual ~ReloadConf();
		virtual int doRequest(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt )  ; 
};

#endif

