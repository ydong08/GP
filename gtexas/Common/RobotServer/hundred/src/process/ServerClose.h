//---------------------------------------------------------------------------

#ifndef ServerCloseH
#define ServerCloseH
//---------------------------------------------------------------------------
#include "IProcess.h"

class ServerClose:public IProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
