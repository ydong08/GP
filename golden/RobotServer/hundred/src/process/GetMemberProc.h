//---------------------------------------------------------------------------

#ifndef GetMemberProcH
#define GetMemberProcH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class GetMemberProc:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
