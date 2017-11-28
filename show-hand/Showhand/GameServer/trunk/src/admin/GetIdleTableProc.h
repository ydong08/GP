#ifndef _GetIdleTableProc_H_
#define _GetIdleTableProc_H_


#include "../IProcess.h"
class Table;
class Player;

class GetIdleTableProc  : public IProcess
{
	public:
		GetIdleTableProc(){};
		virtual ~GetIdleTableProc(){};
	 
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;        

	private:
		int sendErrorMsg(ClientHandler* clientHandler, short errcode, const char* errmsg);
		
};
#endif



