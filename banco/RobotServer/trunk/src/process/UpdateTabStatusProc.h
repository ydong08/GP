//---------------------------------------------------------------------------

#ifndef UpdateTabStatusProcH
#define UpdateTabStatusProcH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class UpdateTabStatusProc:public BaseProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
