#ifndef __IProcess_H__
#define __IProcess_H__
#include "Table.h"
#include "Player.h"

#define _NOTUSED(V) ((void) V)

class IProcess
{
//静态函数
public:
    static int GameStart( Table* table);
    static int GameOver( Table* table);
    static int sendRoundCard(Player* player, Table* table, short currRound);
    static int sendPoolinfo(Table* table);
    static int sendGameStartInfo(Player* player, Table* table);
    static int sendGameOverinfo(Player* player, Table* table);
    static int serverPushLeaveInfo(Table* table, Player* leavePlayer, short retno = 1);
    static int serverWarnPlayerKick(Table* table, Player* warnner, short timeCount);
    static int pushPrivateOprate(Table* table);

    static int sendToRobotCard(Player* player, Table* table);

    static int updateDB_UserCoin(Player* player, Table* table);
    static int updateDB_UserCoin(LeaverInfo* player, Table* table);
    static int updateDB_UserCoin(Player* player, Table* table, int MagicSubCoin);
    static int UpdateDBActiveTime(Player* player);

    static int sendTakeSeatInfo(Table* table, Player *player, int action, int seatid, short seq);
    static void sendChatInfo(Table* table, Player* player, int touid, short type, const char* msg, short seq);
    static void sendGiftInfo(Table* table, Player* player, int toseatid, short type, int subid, BYTE sendtoall, short seq);
    static void sendUpdateMoney(Table* table, Player* player);
};

#endif 
