#ifndef _SetServerStatProc_H
#define _SetServerStatProc_H
 
#include "CDLSocketHandler.h"
#include "IProcess.h"

 
class SetServerStatProc :public IProcess
{
	public:
		SetServerStatProc();
		virtual ~SetServerStatProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt) ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt ) ; 
};


#endif

