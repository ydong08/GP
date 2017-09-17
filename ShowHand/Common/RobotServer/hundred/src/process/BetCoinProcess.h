//---------------------------------------------------------------------------

#ifndef BetCoinProcessH
#define BetCoinProcessH
//---------------------------------------------------------------------------
#include "IProcess.h"

class BetCoinProcess:public IProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
