#ifndef _GetServerInfoProc_H
#define _GetServerInfoProc_H
 
#include "CDLSocketHandler.h"
#include "IProcess.h"

 
class GetServerInfoProc :public IProcess
{
	public:
		GetServerInfoProc();
		virtual ~GetServerInfoProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt) ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt) ; 
};


#endif

