#ifndef _GameOverProc_H_
#define _GameOverProc_H_

#include "BaseProcess.h"

class GameOverProc :public BaseProcess
{
	public:
		GameOverProc();
		virtual ~GameOverProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

