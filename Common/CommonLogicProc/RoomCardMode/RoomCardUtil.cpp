//
// Created by user on 2017/4/6.
//
#include <vector>
#include <map>
#include "RoomCardUtil.h"
#include "Utility.h"
#include "RedisLogic.h"
#include "Logger.h"
#include "GameApp.h"
#include "Configure.h"
#include "Packet.h"
#include "ProtocolServerId.h"
#include "MySqlLogic.h"
#include "StrFunc.h"

namespace RoomCardUtil {

    static bool IsExist(const std::vector<std::pair<int, int>> &list, int dest, int&owneruid) {
        bool isExist = false;

        std::vector<std::pair<int, int>>::const_iterator citer = list.begin();
        for (; citer != list.end(); ++citer) {
            if ((*citer).second == dest) {
                isExist = true;
                owneruid = citer->first;
                break;
            }
        }

        return isExist;
    }

    int randomTableID(int gameID, int min /*= 1000*/, int max /*= 9999*/) {
        std::vector<std::pair<int, int>> usedTableIDs;
        if (!RedisLogic::getCardRoomInfoByGameId(Round(), gameID, usedTableIDs)) {
            LOGGER(E_LOG_ERROR) << "Get used table info failed, gameID = " << gameID;
            return -1;
        }

        if (usedTableIDs.size() >= (size_t) Configure::getInstance()->GetMaxTableNumber()) {
            LOGGER(E_LOG_ERROR) << "Sorry, all tables is used!";
            return -2;
        }

        int newTableID = -1;

        // 随机产生
        srand((unsigned) time(0));
        int retryCnter = 0;
        int ownerUid;
        while (true) {
            int tmpID = Utility::RandomRange(min, max);
            if (!IsExist(usedTableIDs, tmpID, ownerUid)) {
                newTableID = tmpID;
                break;
            }

            if (++retryCnter >= 5) {
                break;
            }
        }
        // 顺序获取 (保底方案)
        if (newTableID < 0) {
            for (int idx = min; idx <= max; ++idx) {
                if (!IsExist(usedTableIDs, idx, ownerUid)) {
                    newTableID = idx;
                    break;
                }
            }
        }

        LOGGER(E_LOG_INFO) << "newTableID = " << newTableID;
        return newTableID;
    }

    int getOwnerUidByTableId(int gameID, int tid)
    {
        std::vector<std::pair<int, int>> usedTableIDs;
        if (!RedisLogic::getCardRoomInfoByGameId(Round(), gameID, usedTableIDs)) {
            LOGGER(E_LOG_ERROR) << "Get used table info failed, gameID = " << gameID;
            return -1;
        }
        int ownerUid = -1;
        if (IsExist(usedTableIDs, tid, ownerUid)) {
            LOGGER(E_LOG_ERROR) << "Get used table ownerUid = " << ownerUid;
            return ownerUid;
        }

        return ownerUid;
    }


    bool getRemainMoney(int ownerid, int uid, int64_t& moneyValue, int64_t& maxWins)
    {
        moneyValue = 0;
        maxWins = 0;

        int tmpGameID = 0;
        int tmpTid = 0;
        int tmpServerID = 0;
        int tmpGameLevel = 0;
        int createTime;
        std::string tmpStaticInfo = "";
        std::string tmpDynamicInfo = "";

        if (!RedisLogic::getCardRoomInfo(
                Round(), ownerid,
                tmpGameID, tmpTid, tmpServerID, tmpGameLevel, createTime, tmpStaticInfo, tmpDynamicInfo))
        {
            return false;
        }

        Json::Reader reader;
        Json::Value rootNode;
        if (tmpDynamicInfo.size() < 2 || tmpDynamicInfo[0] != '{' || !reader.parse(tmpDynamicInfo, rootNode))
        {
            // 参数错误
            ULOGGER(E_LOG_ERROR, uid) << "Json format error!";
            return false;
        }

        try {
            if (!rootNode.isMember("player_member") || !rootNode["player_member"].isArray())
                return false;

            std::string tmpMoney, tmpMaxWin;
            Json::Value player_member = rootNode["player_member"];
            for (int i = 0; i < (int)player_member.size(); i++)
            {
                if (player_member[i]["uid"].asInt() == uid) {
                    tmpMoney = player_member[i]["start_money"].asString();
                    tmpMaxWin = player_member[i]["max_wins"].asString();
                    break;
                }
            }
            if (tmpMoney == "")
            {
                return false;
            }
            moneyValue = atoll(tmpMoney.c_str());
            maxWins = atoll(tmpMaxWin.c_str());
        } catch (...) {
            return false;
        }

        return true;
    }

    bool getRoomCardMenu(RedisAccess * redis, std::map<int, int> &pricelist)
    {
        std::string hashkey = StrFormatA("%s_RoomCardConfig", Configure::getInstance()->GetGameTag());
        std::map<std::string, std::string> ret;
        if (redis->HGETALL(hashkey.c_str(), ret) && !ret.empty())
        {
            for (auto it = ret.begin();it != ret.end();it++) {
                int round = atoi(it->first.c_str());
                int price = atoi(it->second.c_str());
                pricelist[round] = price;
            }
            return true;
        }

        return false;
    }

    int getCardRoomPrice(int targetMatchCount)
    {
        int price = targetMatchCount / 3;
        std::map<int, int> pricelist;
        getRoomCardMenu(Server(), pricelist);
        if (pricelist.find(targetMatchCount) != pricelist.end())
            price = pricelist[targetMatchCount];
        return price;
    }

    bool updateUserRoomCard(int uid, int64_t money) {
        OutputPacket respone;
        respone.Begin(CMD_MONEY_UPDATE_COIN);
        respone.WriteInt(uid);
        respone.WriteByte(1);
        respone.WriteInt64(money);
        respone.End();
        if(MoneyServer()->Send(&respone) < 0) {
            _LOG_ERROR_("Send request to MoneyServer Error\n" );
            return false;
        }
        return true;
    }
}