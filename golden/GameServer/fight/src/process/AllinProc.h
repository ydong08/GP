#ifndef _AllinProc_H_
#define _AllinProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class AllinProc :public BaseProcess
{
public:
	AllinProc();
	virtual ~AllinProc();
		
	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendAllinInfoToPlayers(Player* player, Table* table, Player* betcallplayer, int64_t betmoney,Player* nextplayer,short seq);
};

#endif

