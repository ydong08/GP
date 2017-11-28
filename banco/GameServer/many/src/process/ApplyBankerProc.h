#ifndef _ApplyBankerProc_H_
#define _ApplyBankerProc_H_

#include "BaseProcess.h"

class Player;
class Table;

class ApplyBankerProc :public BaseProcess
{
	public:
		ApplyBankerProc();
		virtual ~ApplyBankerProc();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;

	private:
		int sendTabePlayersInfo(Player* player, Table* table, Player* applyer, short seq);

};

#endif

