#ifndef _StartGameProc_H_
#define _StartGameProc_H_

#include "IProcess.h"

class Table;
class Player;

class StartGameProc :public IProcess
{
	public:
		StartGameProc();
		virtual ~StartGameProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendTabePlayersInfo(Player* player, Table* table, short num, Player* starter, short seq);
		int sendTableInfo(Player* player, Table* table);
};

#endif

