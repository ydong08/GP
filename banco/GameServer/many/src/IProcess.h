#ifndef __IProcess_H__
#define __IProcess_H__
#include <string>
#include <map>
#include <json/json.h>
#include "Packet.h"
#include "GameCmd.h"
#include "Table.h"

#define _NOTUSED(V) ((void) V)

class IProcess
{
	public:

		static int GameStart(Table* table);
		static int GameOver(Table* table);

		static int sendTableStatus(Table* table, short time);
		static int rotateBankerInfo(Table* table, Player* banker, Player* prebanker);
		static int NoteBankerLeave(Table* table, Player* banker);

		static int serverPushLeaveInfo(Table* table, Player* leavePlayer, short retno = 1);
		static int updateDB_UserCoin(Player* player, Table* table);
		static int UpdateDBActiveTime(Player* player);
};

#endif 
