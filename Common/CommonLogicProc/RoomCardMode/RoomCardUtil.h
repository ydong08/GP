//
// Created by user on 2017/4/6.
//

#ifndef DD_GAMES_ROOMCARDUTIL_H
#define DD_GAMES_ROOMCARDUTIL_H

#include <stdint.h>
#include "RedisAccess.h"

namespace RoomCardUtil {

    int randomTableID(int gameID, int min = 1000, int max = 9999);

    int getOwnerUidByTableId(int gameId, int tid);

    bool getRemainMoney(int ownerid, int uid, int64_t& moneyValue, int64_t& maxWins);
    /// <summary>
    /// 获取房卡配置信息
    /// std::map<int, int> round==>price
    /// round 可玩局数
    /// price 房卡价格
    /// </summary>
    bool getRoomCardMenu(RedisAccess* redis, std::map<int, int> &pricelist);

    int getCardRoomPrice(int matchCount);

    bool updateUserRoomCard(int uid, int64_t money);

};


#endif //DD_GAMES_ROOMCARDUTIL_H
