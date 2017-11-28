//---------------------------------------------------------------------------

#ifndef BetCoinProcessH
#define BetCoinProcessH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class BetCoinProcess:public BaseProcess
{
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
