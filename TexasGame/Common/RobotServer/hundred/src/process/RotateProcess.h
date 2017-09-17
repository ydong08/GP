//---------------------------------------------------------------------------

#ifndef RotateProcessH
#define RotateProcessH
//---------------------------------------------------------------------------
#include "IProcess.h"

class RotateProcess:public IProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
