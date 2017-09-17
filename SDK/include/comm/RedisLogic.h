

#pragma once

#include <string>
#include <vector>
#include <map>

class RedisAccess;
struct GameServerInfo;

namespace RedisLogic
{
	// offline redis
	bool setPlayerOffline(RedisAccess* redis, int uid, int gameid, int level, int winscore, const std::string& overtime);
	std::string getPlayerOffline(RedisAccess* redis, int uid, int & win_or_lose);
	void delPlayerOffline(RedisAccess* redis, int uid);

	// round redis
	/// <summary>
	/// 根据特权类型以及key类型获取玩家特权等级配置信息：解锁等级，特权名字
	/// hash : Privilege%s:%d
	/// key : Head/Nick
	/// unlocklevel ： 解锁等级
	/// pname : 特权名字
	/// </summary>
	bool getUserPrivilegeConfigInfo(RedisAccess* redis, int ptype, const std::string& key, int& unlocklevel, std::string& pname);
	
	/// <summary>
	/// 获取等级系统配置数据
	/// hash : LevelConfig
	/// </summary>
	bool getLevelConfig(RedisAccess* redis, float& factor, float& levelupFactor, float& fixedFactor, int& startExp);
	
	/// <summary>
	/// 获取玩家的输赢局数
	/// </summary>
	bool getRoundInfo(RedisAccess* redis, int uid, int& win, int& lose);

	/// <summary>
	/// 获取玩家当前装备的特权信息
	/// </summary>
	bool getUserPrivilegeInfo(RedisAccess* redis, int uid, uint8_t& is_open, std::string& now_nick, std::string& now_head);

	/// <summary>
	/// 设置玩家当前是否开启特权
	/// </summary>
	bool setUserPrivilegeOpen(RedisAccess* redis, int uid, uint8_t open);

	/// <summary>
	/// 记录玩家当前装备昵称
	/// </summary>
	bool setUserPrivilegeNick(RedisAccess* redis, int uid, const std::string& name);

	/// <summary>
	/// 记录玩家当前装备头像名
	/// </summary>
	bool setUserPrivilegeHead(RedisAccess* redis, int uid, const std::string& head);

	// operation redis
	/// <summary>
	/// 更新玩家扣税信息
	/// </summary>
	bool UpdateTaxRank(RedisAccess* redis, short pid, short serverid, short gameid, short level, short tid, int uid, int nTax);
	/// <summary>
	/// 记录机器人赢钱信息
	/// </summary>
	bool AddRobotWin(RedisAccess* redis, short pid, short serverid, int winmoney);

	// 其他redis操作
	/// <summary>
	/// 获取指定桌子id总共进行的局数
	/// tableid不带serverid
	/// </summary>
	bool getTablePlayCount(RedisAccess* redis, int serverid, int tableid, int& playcount);
	/// <summary>
	/// 更新指定桌子玩的局数
	/// </summary>
	bool updateTablePlayCount(RedisAccess* redis, int serverid, int tableid, int playcount);
	/// <summary>
	/// 添加服务器信息
	/// </summary>
	bool addServerInfo(RedisAccess* redis, const GameServerInfo& info);
	/// <summary>
	/// 更新服务器玩家人数
	/// </summary>
	bool updateCurrUserNum(RedisAccess* redis, const GameServerInfo& info);
	/// <summary>
	/// 更新服务器数据
	/// </summary>
	bool updateServerMember(RedisAccess* redis, const GameServerInfo& info);
	/// <summary>
	/// 删除服务器数据
	/// </summary>
	bool delServer(RedisAccess* redis, int svrid);
	/// <summary>
	/// 获取所有server id信息
	/// </summary>
	bool getAllServerId(RedisAccess* redis, std::vector<int>& svrids);
	/// <summary>
	/// 根据server id获取服务器信息
	/// </summary>
	bool getServerInfoById(RedisAccess* redis, int svrid, GameServerInfo& info);
	/// <summary>
	/// 获取游戏类型
	/// </summary>
	bool getGameTypes(RedisAccess* redis, std::vector<int>& games);
	/// <summary>
	/// 设置服务器属性
	/// </summary>
	bool setServerProperty(RedisAccess* redis, int svrid, const char* field, int value);
	bool setServerProperty(RedisAccess* redis, int svrid, const char* field, const char* value);
	/// <summary>
	/// 保存房卡模式房间信息，创建新的房卡时调用
	/// tid -- 桌子id，未打包数据，房卡模式一般使用随机序号，而不是直接使用桌子序号
	/// svrid -- game服务器id
	/// level -- game等级
	/// gameid -- 
	/// uid -- 房主uid
	/// roominfo -- 房间相关配置数据
	/// expiretime -- 过期时间，超过时间后会移除记录，单位秒
	/// </summary>
	bool addCardRoomInfo(RedisAccess *redis, int tid, int svrid, int level, int gameid, int uid, int expiretime,
						 const std::string &roominfo, int createtime);
	/// <summary>
	/// 获取指定游戏id所有创建好的桌子id
	/// gameid -- 指定的gameid
	/// tables -- 获取的桌子列表,key对应桌子创建者的uid，value对应桌子id
	/// </summary>
	bool getCardRoomInfoByGameId(RedisAccess *redis, int gameid, std::vector<std::pair<int, int>> &tables);
	/// <summary>
	/// 清除玩家房卡模式开房数据信息
	/// </summary>
	bool clearCardRoomInfo(RedisAccess *redis, int uid);
	/// <summary>
	/// 更新房卡动态数据
	/// </summary>
	bool updateCardRoomInfo(RedisAccess *redis, int uid, const std::string &info);
	/// <summary>
	/// 获取房卡动态数据
	/// 注意，dynamicinfo有可能为空
	/// </summary>
	bool getCardRoomInfo(RedisAccess *redis, int uid, int &gameid, int &tid, int &svrid, int &level, int &createtime,
						 std::string &roominfo, std::string &dynamicinfo);
	/// <summary>
	/// 获取房卡配置信息
    /// std::map<int, int> round==>price
 	/// round 可玩局数
	/// price 房卡价格
	/// </summary>
	bool getRoomCardConfig(RedisAccess* redis, std::map<int, int> &pricelist);

	/// </summary>
	/// 获取玩家当前创建的房卡hash
	/// </summary>
	std::string getCardRoomHashKey(int uid);
}