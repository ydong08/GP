//---------------------------------------------------------------------------

#ifndef LogoutProcessH
#define LogoutProcessH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class LogoutProcess:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
