#ifndef __BASE_GAME_H_
#define __BASE_GAME_H_

#include "Typedef.h"
#include <string>
#include <vector>

#define MAX_POKER_CARD_SIZE   (52)

class GameUtil {
	//Singleton(GameUtil)

public:
	static std::string PokerIntToString(BYTE value);
	static std::string JinhuaCardType(int value);

	// totalMoney : 一共多少钱
	// discount   : 扣多少钱
	// tax        : 抽成
	static void CalcSysWinMoney(int64_t& totalMoney, int& discount, int tax=5);

	/// <summary>
	/// 根据游戏赢钱数量，生成对应的小喇叭通知内容
	/// </summary>
	/// <returns></returns>
	static bool getDisplayMessage(std::string &strTrumpt, int nGame, int nLevel, const char *pPlayName, int64_t winmoney,
		int64_t wincoin1, int64_t wincoin2, int64_t wincoin3, int64_t wincoin4);

	/// <summary>
	/// 生成邀请玩家小喇叭通知内容
	/// </summary>
	static bool getInvitePlayerTrumpt(std::string &strTrumpt, int nGame, int nLevel, int nTable, int nCurCount, int nMaxCount);
	static bool getInvitePlayerTrumpt(std::string &strTrumpt, int nGame, int nAfterSecond);

	/// <summary>
	/// 根据相关参数拼接离线字符串
	/// </summary>
	/// <param name="gameid"></param>
	/// <param name="level"></param>
	/// <param name="uid"></param>
	/// <param name="winscore"></param>
	/// <param name="overtime"></param>
	/// <returns></returns>
	static std::string makeOfflineResultStr(int gameid, int level, int uid, int winscore, const std::string& overtime);
	
	/// <summary>
	/// 打包服务器Id和桌子Id (注：桌号太大的时候，数据会被裁剪掉，导致解包得不到正确的结果)
	/// </summary>
	/// <param name="readTableId">真正的桌子Id</param>
	/// <param name="serverId">真正的桌子Id</param>
	/// <returns>打包后的桌子号</returns>
	static int encodeTableId(int readTableId);
	static int encodeTableId(int readTableId, int serverId);
	
	/// <summary>
	/// 解包桌子Id
	/// </summary>
	/// <param name="encodedTableId">经过打包处理的桌子Id</param>
	/// <param name="defaultValue">默认值</param>
	/// <returns>真正的桌子号</returns>
	static int decodeTableId(int encodedTableId, int defaultValue = 0);
	static int decodeTableIdEx(int encodedTableId, int defaultValue = 0);
	
	/// <summary>
	/// 解包服务器Id
	/// </summary>
	/// <param name="encodedTableId">经过打包处理的桌子Id</param>
	/// <param name="defaultValue">默认值</param>
	/// <returns>目标服务器标识</returns>
	static int decodeServerId(int encodedTableId, int defaultValue = 0);
	
	/// <summary>
	/// 输出调用堆栈
	/// </summary>
	/// <param name="frameCount">需要输出的栈帧数量</param>
	/// <returns></returns>
	static void printStackTrace(int frameCount = 16);
	
	/// <summary>
	/// 生成不重复的序列
	/// </summary>
	/// <param name="dataPool">数据池</param>
	/// <param name="count">需要的结果数量</param>
	/// <param name="outData">结果序列</param>
	/// <returns></returns>
	static bool generateUniqueList(int count, std::vector<int> &outData);
	static bool generateUniqueList(std::vector<int> &dataPool, int count, std::vector<int> &outData);

private:
	static BYTE m_bPokerCardList[MAX_POKER_CARD_SIZE];
};

#endif //__BASE_GAME_H_
