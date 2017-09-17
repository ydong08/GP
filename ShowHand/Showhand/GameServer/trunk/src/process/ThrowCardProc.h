#ifndef _ThrowCardProc_H_
#define _ThrowCardProc_H_

#include "IProcess.h"

class Table;
class Player;

class ThrowCardProc :public IProcess
{
	public:
		ThrowCardProc();
		virtual ~ThrowCardProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendThrowInfoToPlayers(Player* player, Table* table, Player* throwcallplayer,Player* nextplayer,short seq);
		int PushCardToUser(Table* table);
		int sendCardInfo(Table* table,Player* player);
};

#endif

