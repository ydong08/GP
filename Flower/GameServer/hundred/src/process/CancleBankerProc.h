#ifndef _CancleBankerProc_H_
#define _CancleBankerProc_H_

#include "BaseProcess.h"

class Player;
class Table;

class CancleBankerProc :public BaseProcess
{
public:
	CancleBankerProc();
	virtual ~CancleBankerProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendTabePlayersInfo(Player* player, Table* table, Player* applyer, short seq);
};

#endif

