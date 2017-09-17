#ifndef _Despatch_H
#define _Despatch_H
#include <map>
#include "Packet.h"
#include "BaseProcess.h"
#include "Typedef.h"

class ClientHandler;
 
class Despatch 
{
	Singleton(Despatch);

public:
	int doFrontRequest(CDLSocketHandler* clientHandler,  InputPacket* pPacket);
	int doBackResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket);
	//void deleteCallBackParm(int seq, int uid = 0);
private:
	//unsigned short seq;
	//Context* context;
};


#endif

