#ifndef _EnterPrivateRoomProc_H_
#define _EnterPrivateRoomProc_H_

#include "IProcess.h"

class Table;
class Player;

class EnterPrivateRoomProc :public IProcess
{
	public:
		EnterPrivateRoomProc();
		virtual ~EnterPrivateRoomProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendTabePlayersInfo(Player* player, Table* table, short num, int comuid, short seq);
};

#endif

