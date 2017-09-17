#ifndef _GetPlayInfo_H_
#define _GetPlayInfo_H_


#include "BaseProcess.h"
class Table;
class Player;
class BaseClientHandler;

class GetPlayInfo  : public BaseProcess
{
	public:
		GetPlayInfo();
		~GetPlayInfo();
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;        

	private:
		int sendErrorMsg(CDLSocketHandler* clientHandler, short errcode, const char* errmsg);
		
};
#endif



