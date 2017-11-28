#ifndef _BetCallProc_H_
#define _BetCallProc_H_

#include "BaseProcess.h"

class Table;
class Player;

class BetCallProc :public BaseProcess
{
	public:
		BetCallProc();
		virtual ~BetCallProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
	private:
		int sendCallInfoToPlayers(Player* player, Table* table, Player* betcallplayer, int64_t betmoney,Player* nextplayer,short seq);
		int PushCardToUser(Table* table);
		int sendCardInfo(Table* table,Player* player);
};

#endif

