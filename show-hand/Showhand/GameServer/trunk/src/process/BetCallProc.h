#ifndef _BetCallProc_H_
#define _BetCallProc_H_

#include "IProcess.h"

class Table;
class Player;

class BetCallProc :public IProcess
{
	public:
		BetCallProc();
		virtual ~BetCallProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendCallInfoToPlayers(Player* player, Table* table, Player* betcallplayer, int64_t betmoney,Player* nextplayer,short seq);
		int PushCardToUser(Table* table);
		int sendCardInfo(Table* table,Player* player);
};

#endif

