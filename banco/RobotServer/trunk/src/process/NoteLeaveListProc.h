//---------------------------------------------------------------------------

#ifndef NoteLeaveListProcH
#define NoteLeaveListProcH
//---------------------------------------------------------------------------
#include "BaseProcess.h"

class NoteLeaveListProc:public BaseProcess
{
private:
   virtual int doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
   virtual int doResponse(CDLSocketHandler* client, InputPacket* pPacket, Context* pt ) ;
};
#endif
