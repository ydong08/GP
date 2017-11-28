#ifndef _ReloadConf_H_
#define _ReloadConf_H_

#include "IProcess.h"

class ReloadConf :public IProcess
{
	public:
		ReloadConf();
		virtual ~ReloadConf();
		virtual int doRequest(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt )  ; 
};

#endif

