//---------------------------------------------------------------------------

#ifndef GameOverProcH
#define GameOverProcH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class GameOverProc:public BaseProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
