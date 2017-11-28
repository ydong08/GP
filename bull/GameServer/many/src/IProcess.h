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
		static int serverWarnPlayerKick(Table* table, Player* warnner, short timeCount);
		static int updateDB_UserCoin(Player* player, Table* table);
		static int UpdateDBActiveTime(Player* player);

		static int NotifyRobot(Table* table, Player* banker, Player* vip, uint8_t bettype, int64_t betmoney);
		static int SaveHundredDiscount(Table* table, int discount);	//保存暗扣信息
};

#endif 
