#ifndef _Despatch_H
#define _Despatch_H
#include <map>
#include "Packet.h"
#include "IProcess.h"
class ClientHandler;
 

class Despatch 
{
public:
	static Despatch* getInstance();
	virtual ~Despatch(){};
	int doFrontRequest(CDLSocketHandler* clientHandler,  InputPacket* pPacket);
	int doBackResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket);
	void deleteCallBackParm(int seq);

	Context* getContext(unsigned short sn)
	{
		return &context[sn];
	}
private:
	unsigned short seq;
	Context* context ;
	Despatch();
};


#endif

