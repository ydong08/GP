#ifndef _Dispatch_H
#define _Dispatch_H
#include <map>
#include "Packet.h"
#include "BaseProcess.h"
#include "CDLSocketServer.h"
#include "CDLSocketHandler.h"
#include "Typedef.h"

class Dispatch
{
	typedef std::set<uint16_t>		 SET_SEQ_T;
	typedef std::map<int, SET_SEQ_T> MAP_NETFD_SEQ_T;
	
	Singleton(Dispatch);

public:
	int doFrontRequest(CDLSocketHandler* clientHandler,  InputPacket* pPacket);
	int doBackResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket);
	
	void clearNetfdSeq(int netfd);
	
	void initContext(Context* pt , unsigned int seq, BaseProcess* processer, CDLSocketHandler* clientHandler, InputPacket* pPacket);
	void clearContext(Context* pt, int netfd);
    
    void deleteCallBackParm(unsigned int seq);

private:
	void addNetfdSeq(int netfd, uint16_t seq);
	void delNetfdSeq(int netfd, uint16_t seq);

	uint16_t m_seqCount;
	Context* m_context;
	MAP_NETFD_SEQ_T m_mapFdSeq; //netfd set<seq>
};

#endif

