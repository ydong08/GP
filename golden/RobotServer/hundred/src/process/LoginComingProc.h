//---------------------------------------------------------------------------

#ifndef LoginComingProcH
#define LoginComingProcH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class LoginComingProc:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
