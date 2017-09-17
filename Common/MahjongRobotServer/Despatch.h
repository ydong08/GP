#ifndef _Despatch_H
#define _Despatch_H

class ClientHandler;
class Context;
class InputPacket;
class CDLSocketHandler;

class Despatch 
{
public:
	static Despatch* getInstance();
	virtual ~Despatch(){};
	int doFrontRequest(CDLSocketHandler* clientHandler,  InputPacket* pPacket);
	int doBackResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket);
	void deleteCallBackParm(int seq, int uid = 0);
private:
	unsigned short seq;
	Context* context ; 
	Despatch();
};


#endif

