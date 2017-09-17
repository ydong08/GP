//---------------------------------------------------------------------------

#ifndef CancleProcessH
#define CancleProcessH
//---------------------------------------------------------------------------
#include "IProcess.h"

class CancleProcess:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
