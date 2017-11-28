#ifndef _ChatProcess_H_
#define _ChatProcess_H_

#include "BaseProcess.h"

class Table;
class Player;
class ChatProcess :public BaseProcess
{
public:
	ChatProcess();
	virtual ~ChatProcess();

	int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

private:
	int sendChatInfo(Table* table, Player* player, int touid, short type, const char* msg, short seq);
};

#endif

