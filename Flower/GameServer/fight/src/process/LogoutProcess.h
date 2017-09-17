#ifndef _LogoutProcess_H_
#define _LogoutProcess_H_

#include "BaseProcess.h"

class Table;
class Player;

class LogoutProcess : public BaseProcess
{
public:
	LogoutProcess();
	virtual ~LogoutProcess();

	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendPlayersLogoutInfo(Table* table, Player* leavePlayer);
};

#endif
