#ifndef _GetFreePlayer_H_
#define _GetFreePlayer_H_


#include "BaseProcess.h"
class Table;
class Player;
class BaseClientHandler;

class GetFreePlayer  : public BaseProcess
{
	public:
		GetFreePlayer(){};
		virtual ~GetFreePlayer(){};
	 
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;        

	private:
		int sendErrorMsg(BaseClientHandler* clientHandler, short errcode, const char* errmsg);
		
};
#endif



