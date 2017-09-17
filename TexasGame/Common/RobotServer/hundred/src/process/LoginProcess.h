//---------------------------------------------------------------------------

#ifndef LoginProcessH
#define LoginProcessH
//---------------------------------------------------------------------------
#include "IProcess.h"

class LoginProcess:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
