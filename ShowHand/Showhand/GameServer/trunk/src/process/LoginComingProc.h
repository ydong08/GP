#ifndef _LoginComingProc_H_
#define _LoginComingProc_H_

#include "IProcess.h"

class Table;
class Player;

class LoginComingProc :public IProcess
{
	public:
		LoginComingProc();
		virtual ~LoginComingProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendTabePlayersInfo(Player* player, Table* table, short num, int comuid, short seq);
		int sendTableInfo(Player* player, Table* table);
};

#endif

