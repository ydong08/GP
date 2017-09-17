#ifndef _CheckProcess_H_
#define _CheckProcess_H_

#include "BaseProcess.h"

class Table;
class Player;

class CheckProcess :public BaseProcess
{
public:
	CheckProcess();
	virtual ~CheckProcess();
		
	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendCheckInfoToPlayers(Player* player, Table* table, Player* lookcallplayer, short seq);
};

#endif

