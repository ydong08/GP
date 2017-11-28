#ifndef _LoginComingProc_H_
#define _LoginComingProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class LoginComingProc :public BaseProcess
{
public:
	LoginComingProc();
	virtual ~LoginComingProc();

	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendTabePlayersInfo(Player* player, Table* table, short num, int comuid, short seq);
	int sendTableInfo(Player* player, Table* table);
};

#endif

