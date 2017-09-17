#ifndef _UpdateMoneyProc_H_
#define _UpdateMoneyProc_H_


#include "../IProcess.h"
class Table;
class Player;

class UpdateMoneyProc  : public IProcess
{
	public:
		UpdateMoneyProc(){};
		virtual ~UpdateMoneyProc(){};
	 
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;        

	private:
		int sendErrorMsg(ClientHandler* clientHandler, short errcode, char* errmsg);
		
};
#endif



