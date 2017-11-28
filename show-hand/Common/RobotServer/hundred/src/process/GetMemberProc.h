//---------------------------------------------------------------------------

#ifndef GetMemberProcH
#define GetMemberProcH
//---------------------------------------------------------------------------
#include "IProcess.h"

class GetMemberProc:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
