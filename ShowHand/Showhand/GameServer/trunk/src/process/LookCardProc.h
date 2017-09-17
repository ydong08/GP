#ifndef _LookCardProc_H_
#define _LookCardProc_H_

#include "IProcess.h"

class Table;
class Player;

class LookCardProc :public IProcess
{
	public:
		LookCardProc();
		virtual ~LookCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendLookInfoToPlayers(Player* player, Table* table, Player* lookcallplayer,Player* nextplayer, short seq);
		int PushCardToUser(Table* table);
		int sendCardInfo(Table* table,Player* player);
};

#endif

