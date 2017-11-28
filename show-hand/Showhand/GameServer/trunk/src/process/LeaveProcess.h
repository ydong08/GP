#ifndef _LeaveProcess_H_
#define _LeaveProcess_H_


#include "IProcess.h"
class Table;
class Player;

class LeaveProcess:public IProcess
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
