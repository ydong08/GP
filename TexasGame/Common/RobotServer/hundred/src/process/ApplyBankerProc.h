//---------------------------------------------------------------------------

#ifndef ApplyBankerProcH
#define ApplyBankerProcH
//---------------------------------------------------------------------------
#include "IProcess.h"

class ApplyBankerProc:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
