#ifndef _UpdateMemberProc_H
#define _UpdateMemberProc_H
 
#include "CDLSocketHandler.h"
#include "IProcess.h"

 
class UpdateMemberProc :public IProcess
{
	public:
		UpdateMemberProc();
		virtual ~UpdateMemberProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ; 
};


#endif

