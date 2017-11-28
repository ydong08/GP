#ifndef _GetRoomInfoProcess_H_
#define _GetRoomInfoProcess_H_


#include "../IProcess.h"
class Table;
class Player;

class GetRoomInfoProcess  : public IProcess
{
	public:
		GetRoomInfoProcess(){};
		virtual ~GetRoomInfoProcess(){};
	 
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt );
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt ) ;       

	private:
		int doGetRoomInfo(CDLSocketHandler* clientHandler,int PageNo, int PageNum);
};
#endif



