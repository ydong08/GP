#include "IProcess.h"
#include "Logger.h"

int IProcess::send(CDLSocketHandler* clientHandler, OutputPacket* outPacket, bool isEncrypt)
{
	if (clientHandler && outPacket)
	{
#ifndef CRYPT
		if (isEncrypt)
			outPacket->EncryptBuffer();
#endif

		HallHandler			*pHallHandler = dynamic_cast<HallHandler *>(clientHandler);

		if (NULL == pHallHandler)
		{
			assert(false);
			return -1;
		}

		return pHallHandler->Send(outPacket, isEncrypt);
	}
	return -1;
}