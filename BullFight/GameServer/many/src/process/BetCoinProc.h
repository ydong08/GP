#ifndef _BetCoinProc_H_
#define _BetCoinProc_H_

#include "BaseProcess.h"
#include "Player.h"
#include "Table.h"

class BetCoinProc :public BaseProcess
{
	public:
		BetCoinProc();
		virtual ~BetCoinProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
	private:
		int sendTabePlayersInfo(Player* player, Table* table, Player* beter, short bettype, int64_t betmoney, int x, int y, short seq);
};

#endif

