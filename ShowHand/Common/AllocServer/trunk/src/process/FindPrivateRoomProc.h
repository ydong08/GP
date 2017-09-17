#ifndef _FindPrivateRoomProc_H_
#define _FindPrivateRoomProc_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class FindPrivateRoomProc :public IProcess
{
	public:
		FindPrivateRoomProc();
		virtual ~FindPrivateRoomProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif

