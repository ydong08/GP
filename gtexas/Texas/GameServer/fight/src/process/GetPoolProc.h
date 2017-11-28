#ifndef _GetPoolProc_H_
#define _GetPoolProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class GetPoolProc :public BaseProcess
{
	public:
		GetPoolProc();
		virtual ~GetPoolProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
};

#endif

