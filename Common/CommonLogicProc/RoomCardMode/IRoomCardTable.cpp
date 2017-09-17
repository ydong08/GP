//
// Created by user on 2017/4/8.
//

#include "IRoomCardTable.h"
#include "RedisLogic.h"
#include "MySqlLogic.h"
#include "GameApp.h"
#include "Logger.h"
#include "Configure.h"
#include "Utility.h"
#include "RoomCardUtil.h"
#include "Player.h"
#include "Table.h"
#include "CoinConf.h"
#include "base64.h"

IRoomCardTable::IRoomCardTable() {
    resetRoomData();
}

IRoomCardTable::IRoomCardTable(int id) : m_nId(id) {
    m_nStartTime = 0;
    m_nLastActiveTime = 0;
}

IRoomCardTable::~IRoomCardTable() {

}

void IRoomCardTable::resetRoomData() {
    m_bCharged = false;
    m_nOwnerUid = 0;
    m_nId = -1;
    m_nTotalRound = 0;
    m_nCurrentRound = 0;
    m_nFinishRound = 0;
    m_nDissolveSponsorUid = 0;
    m_nAcceptDissolveCount = 0;
    m_nTotalDissolveCount = 0;
    m_nStartTime = 0;
    m_nLastActiveTime = 0;
    m_PlayerMember.clear();
}

bool IRoomCardTable::isOwnerInRoom()
{
    for (int idx = 0; idx < m_nNumPlayer; ++idx)
    {
        Player *p = getPlayerBySeatId(idx);
        if (p && p->id == m_nOwnerUid)
        {
            return true;
        }
    }
    return false;
}

int IRoomCardTable::getPlayerCount()
{
    int count = 0;
    for (int idx = 0; idx < m_nNumPlayer; ++idx)
    {
        Player *p = getPlayerBySeatId(idx);
        if (p)
            count++;
    }
    return count;
}

int IRoomCardTable::getPlayerIndex(Player* player)
{
    return isMember(player->id) ? m_PlayerMember[player->id].index : player->m_nTabIndex;
}

int IRoomCardTable::setOwner(int ownerid, int expiretime, std::string& jsonOptions, bool restoreMode) {
    int retCode = 0;

    m_nOwnerUid = ownerid;
    m_nExpireTime = expiretime;

    // 保存分配结果信息 (供大厅转发消息用)
    if (!restoreMode)
    {
        m_nCreateTime = time(NULL);
        if (!RedisLogic::addCardRoomInfo(
                Round(),
                m_nId,
                Configure::getInstance()->m_nServerId,
                Configure::getInstance()->m_nLevel,
                Configure::getInstance()->m_nGameID,
                m_nOwnerUid,
                m_nExpireTime,
                jsonOptions,
                m_nCreateTime))
        {
            // 保存房间信息失败
            retCode = -26;
        }

        if (!MySqlLogic::addCardRoomHistory(
                MySqlServer(),
                m_nOwnerUid,
                m_nId,
                GAME_ID,
                m_nTotalRound,
                m_nCreateTime,
                RoomCardUtil::getCardRoomPrice(m_nTotalRound),//cost
                m_nNumPlayer))
        {
            // 保存房间历史记录失败
            retCode = -29;
        }
        m_PlayerMember.clear();
    }

    updateActiveTime();
    return retCode;
}

int IRoomCardTable::resetOwner() {
    int retCode = 0;

    // 清除分配结果信息 (供大厅转发消息用)
    bool bRet = RedisLogic::clearCardRoomInfo(Round(), m_nOwnerUid);
    if (!bRet)
    {
        retCode = -1;
    }
    ULOGGER(E_LOG_DEBUG, m_nOwnerUid) << "clearCardRoomInfo ret:" << bRet;
    resetRoomData();

    return retCode;
}

void IRoomCardTable::updateCardRoomInfo() {
    Json::Value rootNode, rinfo;
    // shared info
    rootNode["actual_match_count"] = this->m_nFinishRound;
    rootNode["is_charged"] = this->m_bCharged ? 1 : 0;

    Json::Value player_member;
    // player info
    for (int idx = 0; idx < m_nNumPlayer; ++idx)
    {
        Json::Value pi;
        Player *p = getPlayerBySeatId(idx);
        if (!p) continue;
        pi["uid"] = p->id;
        pi["start_money"] = p->m_lMoney;
        pi["max_wins"] = p->m_lMaxWinScore;
        pi["win_times"] = p->m_nWinTimes;
        pi["index"] = getPlayerIndex(p);
        player_member.append(pi);
        if (isMember(p->id))
            m_PlayerMember[p->id].win_times = p->m_nWinTimes;
    }
    rootNode["player_member"] = player_member;
    RedisLogic::updateCardRoomInfo(Round(), m_nOwnerUid, Utility::DumpJson(rootNode));
}

void IRoomCardTable::updateCardRoomHistory(bool bDissolve)
{
    // 更新房卡每局记录
    int status = (isMatchFinished() || bDissolve) ? 1 : 0;
    if (!bDissolve) {
        std::vector<MySqlLogic::CardRoomPlayerInfo> matchRecordInfo;
        for (int i = 0; i < GAME_PLAYER; i++)
        {
            MySqlLogic::CardRoomPlayerInfo tmpInfo;
            {
                Player * p = getPlayerBySeatId(i);
                if (!p) continue;
                tmpInfo.uid = p->id;
                tmpInfo.index = getPlayerIndex(p);
                tmpInfo.name = Utility::getCardHexStr((unsigned char*)p->name.data(), p->name.size());
                tmpInfo.score = p->m_lFinalWinScore;
            }
            matchRecordInfo.push_back(tmpInfo);
        }
        MySqlLogic::addCardRoomRecord(MySqlServer(), m_nOwnerUid, m_nId, GAME_ID, m_nFinishRound, m_nTotalRound,
                                      m_nCreateTime, m_nStartTime, time(NULL), status, GAME_PLAYER, matchRecordInfo, Utility::DumpJson(m_jsGamePlayRecord));
    }

    int startmoney = CoinConf::getInstance()->get_start_money(10000);
    // 更新房卡累计成绩
    std::vector<MySqlLogic::CardRoomPlayerInfo> matchHistoryInfo;
    if (!bDissolve) {
        for (int i = 0; i < GAME_PLAYER; i++)
        {
            MySqlLogic::CardRoomPlayerInfo tmpInfo;
            {
                Player * p = getPlayerBySeatId(i);
                if (!p) continue;
                tmpInfo.uid = p->id;
                tmpInfo.index = getPlayerIndex(p);
                tmpInfo.name = Utility::getCardHexStr((unsigned char*)p->name.data(), p->name.size());
                tmpInfo.score = p->m_lMoney - startmoney;
            }
            matchHistoryInfo.push_back(tmpInfo);
        }
    }

    ULOGGER(E_LOG_DEBUG, m_nOwnerUid) << "status:" << status;
    if (MySqlLogic::updateCardRoomHistory(MySqlServer(), m_nOwnerUid, m_nId, GAME_ID, m_nCreateTime,
                                          m_nFinishRound, status, matchHistoryInfo))
    {
        LOGGER(E_LOG_ERROR) << "Update cardroom history success!";
    }
    else
    {
        LOGGER(E_LOG_ERROR) << "Update cardroom history failed!";
    }
}

void IRoomCardTable::chargeRoomCard() {
    // 扣除房卡
    if (!m_bCharged)
    {
        // notify client.
        int hallid = 0, pid = 0;
        for (int idx = 0; idx < m_nNumPlayer; ++idx)
        {
            Player* p = getPlayerBySeatId(idx);
            if (!p) continue;
            if (p->id == m_nOwnerUid) {
                pid = p->pid;
                hallid = p->m_nHallid;
            }

            IPlayerInfo  ipi;
            ipi.uid = p->id;
            ipi.index = idx;
            ipi.max_wins = p->m_lMaxWinScore;
            ipi.start_money = p->m_lMoney;
            m_PlayerMember[p->id] = ipi;
        }
        int chargedCount = RoomCardUtil::getCardRoomPrice(m_nTotalRound);
        int remainCount = 0;

        RoomCardUtil::updateUserRoomCard(m_nOwnerUid, -chargedCount);
        MySqlLogic::addRoomCardChargeLog(MySqlServer(), m_nOwnerUid, m_nId, GAME_ID, pid, -chargedCount, 0);
        m_bCharged = true;
        updateCardRoomInfo();
        ULOGGER(E_LOG_ERROR, m_nOwnerUid) << "Save room passport success, uid = " << m_nOwnerUid << ", charge_count = " << chargedCount;

//        Table::Send_CardRoom_Info(hallid, 0, "", m_nOwnerUid, 1, chargedCount, remainCount);
    }
}

void IRoomCardTable::updateActiveTime() {
    m_nLastActiveTime = time(NULL);
}

void IRoomCardTable::recycleTableIfIdle()
{
    //如果房间号创建，并且没有玩家在桌子上
    if (m_nId > 0 && getPlayerCount() == 0) {
        //如果时间超过20分钟
        if (m_nLastActiveTime != 0 && time(NULL) - m_nLastActiveTime > 1200 ) {
            LOGGER(E_LOG_DEBUG) << "recycle room id:" << m_nId;
            resetRoomData();
        }
    }
}

void IRoomCardTable::recordTableInfo()
{
    for (int i = 0; i < m_nNumPlayer; i++) {
        Player* p = getPlayerBySeatId(i);
        if (!p) continue;
        char key[64] = { 0 };
        sprintf(key , "%d" , p->id);
        Json::Value info;
        info["seatid"] = p->m_nTabIndex;
        info["name"] = Utility::getCardHexStr((unsigned char*)p->name.data(), p->name.size());
        info["money"] = p->m_lMoney;
        m_jsGamePlayRecord["tableinfo"][key] = info;
    }
    m_jsGamePlayRecord["name_encode"] = 1;
    Json::Value option;
    Json::Reader reader;
    reader.parse(getTableOptions(1).c_str(), option);
    m_jsGamePlayRecord["tableoption"] = option;
}

void IRoomCardTable::recordServerInfo()
{
    m_jsGamePlayRecord["starttime"] = m_nStartTime;
    m_jsGamePlayRecord["endtime"] = time(NULL);
    m_jsGamePlayRecord["tid"] = m_nId;
    m_jsGamePlayRecord["svid"] = Configure::getInstance()->m_nServerId;
    m_jsGamePlayRecord["rlevel"] = Configure::getInstance()->m_nLevel;
}

void IRoomCardTable::recordResult()
{
    for (int i = 0; i < m_nNumPlayer; i++) {
        Player* p = getPlayerBySeatId(i);
        if (!p) continue;
        char key[64] = { 0 };
        sprintf(key , "%d" , p->id);
        Json::Value info;
        info["winscore"] = p->m_lFinalWinScore;
        info["wintimes"] = p->m_nWinTimes;
        info["totalwin"] = p->m_lTotalWinScore;
        info["maxwins"] = p->m_lMaxWinScore;
        info["money"] = p->m_lMoney;
        m_jsGamePlayRecord["result"][key] = info;
    }
}

void IRoomCardTable::recordOperator(int uid , std::string& operation, unsigned char value)
{

}
