#ifndef _LoginComingProc_H_
#define _LoginComingProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class LoginComingProc :public BaseProcess
{
	public:
		LoginComingProc();
		virtual ~LoginComingProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendTabePlayersInfo(Player* player, Table* table, short num, int comuid, short seq);
};

#endif

