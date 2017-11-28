#ifndef _GET_FREE_PLAYER_PROC_H_
#define _GET_FREE_PLAYER_PROC_H_

#include "BaseProcess.h"

class GetFreePlayerProc :public BaseProcess
{
	public:
		GetFreePlayerProc();
		virtual ~GetFreePlayerProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,  Context* pt);
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt); 
};

#endif //End for _GET_FREE_PLAYER_PROC_H_

