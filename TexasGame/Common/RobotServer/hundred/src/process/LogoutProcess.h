//---------------------------------------------------------------------------

#ifndef LogoutProcessH
#define LogoutProcessH
//---------------------------------------------------------------------------
#include "IProcess.h"

class LogoutProcess:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
