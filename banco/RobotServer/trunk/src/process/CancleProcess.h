//---------------------------------------------------------------------------

#ifndef CancleProcessH
#define CancleProcessH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class CancleProcess:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
