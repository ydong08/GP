//---------------------------------------------------------------------------

#ifndef GetDetailProcessH
#define GetDetailProcessH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class GetDetailProcess:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
