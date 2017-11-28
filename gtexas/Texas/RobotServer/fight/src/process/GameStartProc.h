#ifndef _GameStartProc_H_
#define _GameStartProc_H_

#include "IProcess.h"

class GameStartProc :public IProcess
{
	public:
		GameStartProc();
		virtual ~GameStartProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

