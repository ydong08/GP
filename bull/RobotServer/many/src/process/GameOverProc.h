//---------------------------------------------------------------------------

#ifndef GameOverProcH
#define GameOverProcH
//---------------------------------------------------------------------------
#include "IProcess.h"

class GameOverProc:public IProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
