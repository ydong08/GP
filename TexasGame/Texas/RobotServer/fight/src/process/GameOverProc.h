#ifndef _GameOverProc_H_
#define _GameOverProc_H_

#include "IProcess.h"

class GameOverProc :public IProcess
{
	public:
		GameOverProc();
		virtual ~GameOverProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

