#ifndef _AllinProc_H_
#define _AllinProc_H_

#include "IProcess.h"

class Table;
class Player;

class AllinProc :public IProcess
{
	public:
		AllinProc();
		virtual ~AllinProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendAllinInfoToPlayers(Player* player, Table* table, Player* betcallplayer, int64_t betmoney,Player* nextplayer,short seq);
		int PushCardToUser(Table* table);
		int sendCardInfo(Table* table,Player* player);
};

#endif

