
#include "ServerClose.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(SERVER_GAMESERVER_CLOSED, ServerClose);

int ServerClose::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int ServerClose::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	return EXIT;
}

