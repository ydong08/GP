#ifndef _UpdateTableUserCountProc_H
#define _UpdateTableUserCountProc_H
 
#include "CDLSocketHandler.h"
#include "IProcess.h"

 
class UpdateTableUserCountProc :public IProcess
{
	public:
		UpdateTableUserCountProc();
		virtual ~UpdateTableUserCountProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ; 
};


#endif

