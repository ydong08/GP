#ifndef _RaseProcess_H_
#define _RaseProcess_H_

#include "BaseProcess.h"

class Table;
class Player;

class RaseProcess : public BaseProcess
{
public:
	RaseProcess();
	virtual ~RaseProcess();

	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendRaseInfoToPlayers(Player* player, int cmd, Table* table, Player* betcallplayer,int64_t betmoney,Player* nextplayer,short seq);
};

#endif

