#ifndef _SetServerInfoProc_H
#define _SetServerInfoProc_H
 
#include "CDLSocketHandler.h"
#include "IProcess.h"

 
class SetServerInfoProc :public IProcess
{
	public:
		SetServerInfoProc();
		virtual ~SetServerInfoProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ; 
};


#endif

