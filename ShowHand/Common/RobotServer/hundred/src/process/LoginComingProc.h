//---------------------------------------------------------------------------

#ifndef LoginComingProcH
#define LoginComingProcH
//---------------------------------------------------------------------------
#include "IProcess.h"

class LoginComingProc:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
