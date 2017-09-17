#ifndef _StartGameProc_H_
#define _StartGameProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class StartGameProc :public BaseProcess
{
	public:
		StartGameProc();
		virtual ~StartGameProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
	private:
		int sendTabePlayersInfo(Player* player, Table* table, short num, Player* start, short seq);
};

#endif

