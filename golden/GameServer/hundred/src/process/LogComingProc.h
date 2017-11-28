#ifndef _LogComingProc_H_
#define _LogComingProc_H_

#include "BaseProcess.h"

class Player;
class Table;

class LogComingProc :public BaseProcess
{
public:
	LogComingProc();
	virtual ~LogComingProc();
	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendTabePlayersInfo(Table* table, Player* player, short seq);
};

#endif
