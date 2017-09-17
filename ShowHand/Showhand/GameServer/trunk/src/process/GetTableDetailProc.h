#ifndef _GetTableDetailProc_H_
#define _GetTableDetailProc_H_

#include "IProcess.h"

class Table;
class Player;

class GetTableDetailProc :public IProcess
{
	public:
		GetTableDetailProc();
		virtual ~GetTableDetailProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
};

#endif

