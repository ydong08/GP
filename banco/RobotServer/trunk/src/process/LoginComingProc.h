//---------------------------------------------------------------------------

#ifndef LoginComingProcH
#define LoginComingProcH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class LoginComingProc:public BaseProcess
{
public:
	LoginComingProc();

	~LoginComingProc();

   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
