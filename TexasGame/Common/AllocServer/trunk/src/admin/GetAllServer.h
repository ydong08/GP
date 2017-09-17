#ifndef _GetAllServer_H
#define _GetAllServer_H
 
#include "CDLSocketHandler.h"
#include "IProcess.h"

 
class GetAllServer :public IProcess
{
	public:
		GetAllServer();
		virtual ~GetAllServer();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt ) ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt ) ; 
};


#endif

