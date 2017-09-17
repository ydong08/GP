#ifndef __IProcess_H__
#define __IProcess_H__
#include <string>
#include <map>
#include <json/json.h>
#include "CDLSocketHandler.h"
#include "BaseClientHandler.h"
#include "Packet.h"
#include "GameCmd.h"
#include "Table.h"

#define _NOTUSED(V) ((void) V)

class IProcess
{
//静态函数
public:
	static int GameStart( Table* table);
	static int GameOver( Table* table);
	static int sendGameStartInfo(Player* player, Table* table);
	static int sendGameOverinfo(Player* player, Table* table);
	static int serverPushLeaveInfo(Table* table, Player* leavePlayer, short retno = 1);
	static int serverWarnPlayerKick(Table* table, Player* warnner, short timeCount);
	static int pushForbidinfo(Table* table);
	static int sendToRobotCard(Player* player, Table* table);
	static int updateDB_UserCoin(Player* player, Table* table);
	static int updateDB_UserCoin(LeaverInfo* player, Table* table);
	static int UpdateDBActiveTime(Player* player);
    static int NotifyPlayerNetStatus(Table* table, int uid, int signal, int type = 0); //当玩家掉线重连成功后，通知同桌的其他玩家，type=1
        static int RecordWinLoser(int winUid, vector<int>& losers, vector<int64_t>& loseCoin, int loserCount, Table* table);
        static int sendCompareInfoToPlayers(Player* player , Table* table , Player* compareplayer , Player* opponent , short compareRet , Player* nextplayer , int64_t comparecoin , short seq);

        static int ForceCampareCard(Table* table , Player* p1 , Player* p2, short seq, bool isDelayNotify);
};

#endif 
