#ifndef _CheckProcess_H_
#define _CheckProcess_H_

#include "BaseProcess.h"

class Table;
class Player;

class CheckProcess :public BaseProcess
{
	public:
		CheckProcess();
		virtual ~CheckProcess();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )   ;

	private:
		int sendCheckInfoToPlayers(Player* player, Table* table, Player* lookcallplayer,Player* nextplayer, short seq);
		int PushCardToUser(Table* table);
		int sendCardInfo(Table* table,Player* player);
};

#endif

