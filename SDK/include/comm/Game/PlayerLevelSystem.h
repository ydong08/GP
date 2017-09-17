//
//  PlayerLevelSystem.h
//  DD_Servers
//
//  Created by yuzhan on 2016/11/2.
//
//

#ifndef PlayerLevelSystem_h
#define PlayerLevelSystem_h

#include "Typedef.h"
#include <map>
#include <string>

class OutputPacket;
class CDLSocketHandler;

//特权卡
enum PrivilegeCard
{
    CopperCard = 0,         //铜卡
    SilverCard,             //银卡
    GoldenCard,             //金卡
    PlatinumCard,           //白金卡
    DiamondCard,             //钻石卡
    PrivilegeCnt
};

typedef int(*NotifyToClient)(int,  OutputPacket*, bool);

class PlayerLevelSystem
{
    Singleton(PlayerLevelSystem);
    
public:
    //load factor from redis
    void init(NotifyToClient notify);
    //计算玩家当前经验值，判断是否能升级
    //senduid 要发送消息的玩家uid
    //uid 查询经验的玩家id
    //hallid 发送玩家玩家所在大厅id
    //add_play_count 是否增加游戏局数，游戏结束调用时为true，查询玩家经验等级数据时为false
    void calculateExp(int senduid, int uid, int hallid, bool add_play_count = true); // gameserver使用
    //
    void calculateExp(int senduid, int uid, CDLSocketHandler* clientHandler, bool add_play_count = true, bool enctypt = true); //大厅或其他公共服使用
    //
    bool setPrivilegeOpen(int uid, BYTE open);
    bool setPrivilegeNick(int uid, const std::string& name);
    bool setPrivilegeHead(int uid, const std::string& head);
    
    bool getUserPrivilegeInfo(int uid, BYTE& isopen, std::string& name, std::string& head);
    //获取当前配置开启的特权数量
    int getPrivilegeCount();
    // type - nick or head
    void getPrivilegeInfo(int type, PrivilegeCard card, std::string& name, int& unlock);
    
    int getNextLevelTotalExp(int currentLevel);
    
private:
    struct PlayerLevel
    {
        int level;
        int exp;// 总经验
        int currentExp; //当前等级经验
        int playCount;  //游戏局数
        BYTE is_open_privilege;
        std::string now_nick;
        std::string now_head;
        PrivilegeCard card;
        
        PlayerLevel() : level(0), exp(0), playCount(0), currentExp(0), card(CopperCard), is_open_privilege(1)
        {
            
        }
    };
    std::map<int, PlayerLevel> playerLevels;    //玩家等级数据缓存
    static PlayerLevel INVALID_PLAYER_LEVEL;
    
    std::map<int, int> levelExp;
    
    float factor;
    float levelupFactor;
    float fixedFactor;
    int startExp;
    
    struct PrivilegeConfigData
    {
        int type;
        int unlockLevel;
        std::string name;
    };
    PrivilegeConfigData pconfigNickData[PrivilegeCnt];
    PrivilegeConfigData pconfigHeadData[PrivilegeCnt];
    
    NotifyToClient notifyToClient;
    
private:
    //获取玩家当前等级
    //uid 玩家id
    int getLevel(int uid);
    //获取下一级升级所需经验
    //level 当前等级
    int getLevelupExp(int level);
    //本地是否存在，如果没有就需要从redis获取
    bool isPlayerLevelExist(int uid);
    //获取玩家等级数据
    const PlayerLevel& getPlayerLevelData(int uid);
    //
    int calculatePlayerLevel(int play_count);
    //
    int calculatePlayerExp(int play_count);
    //
    PrivilegeCard calculatePrivilegeCard(int level);
    //
    void loadConfigFromRedis();
    //
    void loadConfigFromXml();
    //
    bool combineNotifyMsg(int senduid, OutputPacket& packet, int uid, bool add_play_count);
    // type - nick or head
    bool loadPrivilegeConfigData(PrivilegeCard ptype, int type);
};

#endif /* PlayerLevelSystem_h */
