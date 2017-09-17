//---------------------------------------------------------------------------

#ifndef UpdateTabStatusProcH
#define UpdateTabStatusProcH
//---------------------------------------------------------------------------
#include "IProcess.h"

class UpdateTabStatusProc:public IProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
