//
// Created by user on 2017/4/8.
//

#ifndef DD_GAMES_IROOMCARDTABLE_H
#define DD_GAMES_IROOMCARDTABLE_H

#include <string>
#include <map>
#include "json/json.h"
class Player;


struct IPlayerInfo {
    int uid;
    int index;
    int max_wins;
    int start_money;
    int win_times;
};
// uid==>IPlayerInfo
typedef std::map<int, IPlayerInfo> RoomCardMember;

class IRoomCardTable {
public:
    IRoomCardTable();
    IRoomCardTable(int id);
    virtual ~IRoomCardTable();
    void resetRoomData();
    int getId() { return m_nId; }
    void setId(int id) { m_nId = id; }
    bool hasOwner() { return m_nOwnerUid != 0; }
    bool isMember(int uid) { return m_PlayerMember.find(uid) != m_PlayerMember.end(); }
    bool isOwnerInRoom();
    int getPlayerCount();
    //用户的Index，用于mysql存储
    int getPlayerIndex(Player* player);
    //比赛是否结算
    bool isMatchFinished() { return m_nFinishRound >= m_nTotalRound; }
    //是否同意解散
    bool isDissolveAcceped() { return m_nAcceptDissolveCount >= getPlayerCount()/2; }
    // 更新房间动态信息
    void updateCardRoomInfo();
    // 更新房卡记录
    void updateCardRoomHistory(bool bDissolve = false);
    // 房卡收费
    void chargeRoomCard();
    // 更新活动时间
    void updateActiveTime();
    //回收房间，如果半小时内没用户在房间内游戏
    void recycleTableIfIdle();

    //清除出牌记录信息
    void clearRecordAnalysisInfo() {
        m_jsGamePlayRecord.clear();
    }

    void recordTableInfo();
    void recordServerInfo();
    void recordResult();
    void recordOperator(int uid , std::string& operation, unsigned char value);

    //Interface of IRoomCard
    virtual bool isAllRobot() = 0;
    virtual Player* getPlayerBySeatId(int seatid) = 0;
    virtual std::string getTableOptions(int joinType) = 0;
    virtual int setTableOptions(int roomid, std::string& jsonOptions) = 0;
    virtual int setOwner(int ownerid, int expiretime, std::string& jsonOption, bool restoreMode);
    virtual int resetOwner();

public:
    int m_nId;			//桌子id
    int m_nNumPlayer;	//玩家个数
    int m_nOwnerUid;    //创建者UID
    int m_nExpireTime;  //有效时间（s)
    int m_nCreateTime;  //房屋创建时间
    int m_nLastActiveTime;  //房屋上次活动时间，用于回收计时
    int m_nStartTime;   //每局游戏开始时间
    std::string	m_sRoomPwd;	//自定义密码
    bool m_bCharged;		//是否已经扣除房卡
    int64_t m_nInitalMoney;  //游戏开始的初始金币

    //局数相关
    int	m_nTotalRound;			//目标局数
    int	m_nCurrentRound;		//当前局数
    int	m_nFinishRound;			//实际完成局数
    //解散房间相关信息
    int m_nLastDissolveSponsorTime; //上次请求结算的时间
    int	m_nDissolveSponsorUid;		//请求解散的玩家id
    int	m_nAcceptDissolveCount;		//同意解散的玩家数量
    int	m_nTotalDissolveCount;		//响应解散的玩家数量

    RoomCardMember m_PlayerMember;

    //游戏记录
    Json::Value m_jsGamePlayRecord;
    int m_nStep;
};

#endif //DD_GAMES_IROOMCARDTABLE_H
