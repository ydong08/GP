
#include "ServerClose.h"

int ServerClose::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int ServerClose::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	return EXIT;
}

