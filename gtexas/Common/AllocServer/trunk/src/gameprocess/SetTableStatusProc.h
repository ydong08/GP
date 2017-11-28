#ifndef _SetTableStatusProc_H
#define _SetTableStatusProc_H
 
#include "CDLSocketHandler.h"
#include "IProcess.h"

 
class SetTableStatusProc :public IProcess
{
	public:
		SetTableStatusProc();
		virtual ~SetTableStatusProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt ) ; 
};


#endif

