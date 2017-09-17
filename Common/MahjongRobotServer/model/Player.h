#ifndef _H_DDZ_LOGIC_H_
#define _H_DDZ_LOGIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h> 
#include <sys/wait.h>
#include <sys/sysinfo.h>

#include <math.h> 

#include <errno.h>

#include <assert.h>

#include "HallHandler.h"
#include "PlayerTimer.h"
#include "GameLogic.h"
#include <vector>

#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define ASSERT(f)  assert(f)
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))

#define INVALID_CHAIR 0xffff

#define STATUS_PLAYER_LOGOUT 0
#define STATUS_PLAYER_LOGIN 1   //登陆，未入座
#define STATUS_PLAYER_COMING 2  //入座
#define STATUS_PLAYER_RSTART 3  //入座，准备开始
#define STATUS_PLAYER_ACTIVE 5  //正在打牌
#define STATUS_PLAYER_OVER 4    //游戏结束

#define  GAME_PLAYER 4
//出牌结果
struct tagOutCardResult
{
	BYTE							cbOperateCode;						//出牌代码
	BYTE							cbOperateCard;						//出牌值
};

//听牌分析结果
struct tagTingCardResult
{
	BYTE bAbandonCount;													//可丢弃牌数
	BYTE bAbandonCard[MAX_COUNT];										//可丢弃牌
	BYTE bTingCardCount[MAX_COUNT];										//听牌数目
	BYTE bTingCard[MAX_COUNT][MAX_COUNT - 1];								//听牌
	BYTE bRemainCount[MAX_COUNT];										//听的牌的剩余数
};
// 处理 机器人 Logic Socket 
class Player
{
public:

	Player();
	virtual ~Player();

	// 记录 玩家 uid 
	int m_Uid;
	short m_UserIndex;

	int m_AllUid[4];

	int m_ServerID;
	
	// 记录 玩家 桌号
	int m_TableId;

	// 记录 坐次
	unsigned char m_TableIndex;
	// 下家索引
	unsigned char m_TableNextIndex;
	// 上家索引
	unsigned char m_TableLastIndex;

	unsigned char m_TableFirstIndex;

	// 手牌缓存
	std::vector<BYTE> m_Card;

	char m_PlayerCardIndex;

	HallHandler * handler;

	unsigned int m_CommonTick;

	// 记录 心跳包 时间
	unsigned int m_HeatTick;

	int m_ConnectState;

	//牌堆信息
	uint16_t					heap_head_;						//牌堆头
	uint16_t					heap_tail_;						//牌堆尾
	uint8_t						heap_info_[4][2];				//牌堆信息

	int64_t						dice_point_;					//筛子点数
	int							banker_pos_;					//庄家方位

	int							current_user_pos_;
	int							left_card_count_;
	int							lianzhuang_count_;

	int							action_;
	BYTE						m_cbActionCard;						//操作牌;

	int							OperateCode_;
	BYTE						OperateCard_;						//操作牌;

	BYTE							m_cbCardIndex[4][MAX_INDEX];	//手中扑克
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存扑克

	BYTE							m_cbWeaveCount[4];		//组合数目
	tagWeaveItem					m_WeaveItemArray[4][MAX_WEAVE];	//组合扑克

	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbOutCardData;					//出牌扑克
	BYTE							m_cbDiscardCount[4];		//丢弃数目
	BYTE							m_cbDiscardCard[4][55];	//丢弃记录

	int								out_card_time_;


	CGameLogic game_logic_; //游戏逻辑
public:
	inline void addPlayCount() {++play_count;};
	inline short getPlayCount() {return play_count;};
public:
	void init();
	int login();
	int logout();
	int heartbeat();
	int callland();
	int Operate();
	int ChangeCard();
	int DefineLack();

	//出牌
	int OnOutCard(BYTE cbOutCard);
	//操作牌
	int OnOperateCard(BYTE cbOperateCode, BYTE cbOperateCard);

	//返回废弃的牌
	bool SearchTingCard(tagTingCardResult &TingCardResult);
	//搜索出牌
	bool SearchOutCard(tagOutCardResult &OutCardResult);

	void GetDefaultChangeCard(uint8_t* changed_cards);
	uint8_t GetDefaultLackColor();

	void reset_data();

public:
	void startHeartTimer(int uid,int timeout);
	void stopHeartTimer();
	void startOutCardTimer(int uid,int timeout);
	void stopOutCardTimer();
	void startLeaveTimer(int timeout);
	void stopLeaveTimer();
	void startChangeCardTimer(int uid, int timeout);
	void stopChangeCardTimer();
	void startDefineLackTimer(int uid, int timeout);
	void stopDefineLackTimer();

public:
	std::string json;
	std::string name;
	short clevel;
	short playNum;
	int64_t money;
	short play_count;
	short status;
	short tstatus;

private:
	int SelectDefaultChangeCardColor(uint8_t *color_count, uint8_t *color_gang);
	void AnalyseHandleColor(uint8_t start_index, uint8_t *color_card_index, tagSimpleItem *simple_items, uint8_t &item_count);

private:
	PlayerTimer timer;
};

#endif

