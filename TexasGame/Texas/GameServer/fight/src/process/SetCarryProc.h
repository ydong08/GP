#ifndef _SetCarryProc_H_
#define _SetCarryProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class SetCarryProc :public BaseProcess
{
	public:
		SetCarryProc();
		virtual ~SetCarryProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;
};

#endif

