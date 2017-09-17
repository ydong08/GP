#ifndef _ThrowCardProc_H_
#define _ThrowCardProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class ThrowCardProc : public BaseProcess
{
public:
	ThrowCardProc();
	virtual ~ThrowCardProc();

	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendThrowInfoToPlayers(Player* player, Table* table, Player* throwcallplayer, Player* nextplayer, short seq);
};

#endif

