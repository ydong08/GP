#ifndef _LeaveProcess_H_
#define _LeaveProcess_H_

#include "BaseProcess.h"

class Table;
class Player;

class LeaveProcess : public BaseProcess
{
public:
	LeaveProcess();
	virtual ~LeaveProcess();

	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendPlayersLeaveInfo(Table* table, Player* leavePlayer, Player* nextplayer, bool isturnthisuid, short seq);
};

#endif
