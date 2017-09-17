//---------------------------------------------------------------------------

#ifndef RotateProcessH
#define RotateProcessH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class RotateProcess:public BaseProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
