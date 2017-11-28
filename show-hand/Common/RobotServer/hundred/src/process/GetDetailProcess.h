//---------------------------------------------------------------------------

#ifndef GetDetailProcessH
#define GetDetailProcessH
//---------------------------------------------------------------------------
#include "IProcess.h"

class GetDetailProcess:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
