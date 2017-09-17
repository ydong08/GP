//---------------------------------------------------------------------------

#ifndef ServerCloseH
#define ServerCloseH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class ServerClose:public BaseProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
