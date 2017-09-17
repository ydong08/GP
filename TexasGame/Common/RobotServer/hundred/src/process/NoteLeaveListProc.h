//---------------------------------------------------------------------------

#ifndef NoteLeaveListProcH
#define NoteLeaveListProcH
//---------------------------------------------------------------------------
#include "IProcess.h"

class NoteLeaveListProc:public IProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
