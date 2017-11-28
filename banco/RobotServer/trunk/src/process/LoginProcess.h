//---------------------------------------------------------------------------

#ifndef LoginProcessH
#define LoginProcessH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class LoginProcess:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
