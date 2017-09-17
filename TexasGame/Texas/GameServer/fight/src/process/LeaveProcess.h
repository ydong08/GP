#ifndef _LeaveProcess_H_
#define _LeaveProcess_H_


#include "BaseProcess.h"
class Table;
class Player;

class LeaveProcess:public BaseProcess
{
	public:
		LeaveProcess();
		virtual ~LeaveProcess();

		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt);		
	private:
		int sendPlayersLeaveInfo(Table* table,Player* leavePlayer, Player* nextplayer, bool isturnthisuid,short seq);
};

#endif
