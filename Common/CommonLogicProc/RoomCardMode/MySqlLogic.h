

#pragma once

#include <string>
#include <vector>

class RedisAccess;
class MySqlAgent;

namespace MySqlLogic
{
	struct CardRoomPlayerInfo
	{
		int uid;					// 玩家id
		int index;			//uid索引
		std::string name;	// 玩家昵称
		int score;				// 玩家输赢金币数量，>0表示赢，<0表示输
	};
	/// </summary>
	/// 添加房间历史记录，创建房间时调用！  RedisLogic::addPassportRoomInfo调用后应该即刻调用
	/// roomowner 房间创建者id
	/// tid 桌子id
	/// gameid 游戏id
	/// totalround 最大局数
	/// createtime 创建时间戳
	/// playercount 每局玩家数量，如郑州麻将人数为4，斗地主为3，等等
	/// </summary>
	bool addCardRoomHistory(MySqlAgent *mysql, int roomowner, int tid, int gameid, int totalround, int createtime,
							int cost, int playercount);
	
	/// </summary>
	/// 更新房卡历史数据，房间解散前调用,需保证在RedisLogic::clearPassportRoomInfo前调用，否则会查询不到创建时间
	/// roomowner 房间创建者id
	/// tid 桌子id
	/// gameid 游戏id
	/// history 历史记录
	/// </summary>
	bool updateCardRoomHistory(MySqlAgent *mysql, int roomowner, int tid, int gameid, int createtime,
							   int finishround, int status, const std::vector<CardRoomPlayerInfo> &history);

	/// </summary>
	/// 添加房间每局历史记录
	/// roomowner 房间创建者id
	/// tid 桌子id
	/// gameid 游戏id
	/// totalround 最大局数
	/// starttime 开始时间戳
	/// endtime 结束时间戳
	/// playercount 每局玩家数量，如郑州麻将人数为4，斗地主为3，等等
	/// </summary>
	bool addCardRoomRecord(MySqlAgent *mysql, int roomowner, int tid, int gameid, int currentround, int totalround,
						   int createtime, int starttime, int endtime, int status, int playercount,
						   const std::vector<CardRoomPlayerInfo> &history, const std::string& playinfo);

	/**
	 * 添加消费房卡记录
	 * @param mysql
	 * @param roomowner
	 * @param tid
	 * @param gameid
	 * @return
	 */
	bool addRoomCardChargeLog(MySqlAgent *mysql, int roomowner, int tid, int gameid, int pid, int64_t winMoney,
							  int64_t currentMoney);


}