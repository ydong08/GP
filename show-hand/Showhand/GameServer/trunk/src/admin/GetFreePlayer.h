#ifndef _GetFreePlayer_H_
#define _GetFreePlayer_H_


#include "../IProcess.h"
class Table;
class Player;

class GetFreePlayer  : public IProcess
{
	public:
		GetFreePlayer(){};
		virtual ~GetFreePlayer(){};
	 
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;        

	private:
		int sendErrorMsg(ClientHandler* clientHandler, short errcode, const char* errmsg);
		
};
#endif



