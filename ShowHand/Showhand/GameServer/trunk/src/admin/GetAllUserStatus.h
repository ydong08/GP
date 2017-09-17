#ifndef _GetAllUserStatus_H_
#define _GetAllUserStatus_H_


#include "../IProcess.h"
class Table;
class Player;

class GetAllUserStatus  : public IProcess
{
	public:
		GetAllUserStatus(){};
		virtual ~GetAllUserStatus(){};
	 
	 	virtual int doRequest(CDLSocketHandler* CDLSocketHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* CDLSocketHandler, InputPacket* inputPacket ,Context* pt )   ;       

	private:
		int sendErrorMsg(CDLSocketHandler* CDLSocketHandler, short errcode, char* errmsg);
};
#endif



