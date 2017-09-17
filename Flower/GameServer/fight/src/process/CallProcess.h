#ifndef _CallProcess_H_
#define _CallProcess_H_

#include "BaseProcess.h"

class Table;
class Player;

class CallProcess :public BaseProcess
{
public:
	CallProcess();
	virtual ~CallProcess();
		
	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendCallInfoToPlayers(Player* player, Table* table, Player* betcallplayer, int64_t betmoney,Player* nextplayer,short seq);
};

#endif

