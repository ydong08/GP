//---------------------------------------------------------------------------

#ifndef ApplyBankerProcH
#define ApplyBankerProcH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class ApplyBankerProc:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
