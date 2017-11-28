#ifndef _TABLE_H
#define _TABLE_H

#include <time.h>
#include "TableTimer.h"
#include "Player.h"
#include "GameLogic.h"
#include "Configure.h"

#define STATUS_TABLE_CLOSE	-1 //桌子关闭
#define STATUS_TABLE_EMPTY	 0 //桌子空
#define STATUS_TABLE_READY	 1 //桌子正在准备

#define STATUS_TABLE_ACTIVE 2	//桌子正在玩牌
#define STATUS_TABLE_OVER	 3	//桌子游戏结束

#define GAME_OVER_TIME		2500

typedef std::set<Player *> PlayerSet;

typedef struct _LeaverInfo
{
	int uid;
	short m_nTabIndex;
	char name[64];
	int64_t betcoin;
	short source;
	short cid;
	short pid;
	short sid;
	bool m_bAllin;
	int64_t m_lMoney;
	int64_t betCoinList[5];
	int m_nWin;
	int m_nLose;
	int m_nRunAway;
	int m_nTie;
	int m_nMagicNum;
	int m_nMagicCoinCount;
	int64_t m_lMaxWinMoney;
	int64_t m_lMaxCardValue;
}LeaverInfo;

typedef struct _ContainBet
{
	int uid;
	int64_t betcoin;
	bool hashandle;
	_ContainBet():uid(0), betcoin(0), hashandle(false)
	{};
}ContainBet;

typedef struct _PoolInfo
{
	ContainBet userBet[GAME_PLAYER];
	int64_t betCoinCount;
}PoolInfo;


class Table
{
public:
	Table();
	virtual ~Table();

	void init();
	void reset();
//内置函数
public:
	inline void closeTable(){this->m_nStatus = STATUS_TABLE_CLOSE;};
	inline bool isClose(){return m_nStatus==STATUS_TABLE_CLOSE;};
	inline bool hasOnePlayer() {return m_nCountPlayer == 1;};
	inline bool isEmpty(){return m_nStatus==STATUS_TABLE_EMPTY || m_nCountPlayer == 0;};
	inline bool isNotFull() {return m_nCountPlayer < Configure::getInstance()->numplayer;};
	inline bool isActive() {return this->m_nStatus == STATUS_TABLE_ACTIVE;};
	inline char* getPassword() { return password; }

	inline void setStartTime(time_t t){StartTime = t;}
	inline time_t getStartTime(){return StartTime;}
	inline void setEndTime(time_t t){EndTime = t;}
	inline time_t getEndTime(){return EndTime;}
	inline void setGameID(const char* id) {strcpy(GameID, id);}
	inline const char* getGameID() {return GameID;}
	inline void setEndType(short type){EndType = type;}
	inline short getEndType(){return EndType;}
	inline void lockTable(){kicktimeflag = true; KickTime = time(NULL);}
	inline void unlockTable(){kicktimeflag = false;}
	inline bool isLock(){return kicktimeflag;}
//行为函数
public:
	void reSetInfo();
	void setTableConf();
	bool isUserInTab(int uid);
	Player* getPlayer(int uid);
	Player* getPlayerInTab(int uid);
	int playerComming(Player* player, int tabindex = -1);
	void playerLeave(int uid);
	void playerLeave(Player* player);
	void setSeatNULL(Player* player);
	//用户在桌子上找到一个座位
	int playerSeatInTab(Player* player);
	//只有一个人能有操作，需要直接发牌结束
	bool isOnlyOneOprate();
	Player* getNextBetPlayer(Player* player, short playeroptype);
	void setPlayerOptype(Player* player, short playeroptype);
	void setPlayerlimitcoin();
	bool isCanGameStart();
	bool isAllReady();
	int64_t getSecondLargeCoin();
	void gameStart();
	bool iscanGameOver();
	bool SelectMaxUser(UserWinList &EndResult);
	int calcCoin();
	//赢钱的用户 输钱的用户ID 输多少钱给这个用户 几个人分
	void setWinnerPool(Player* winner, int loserID, int64_t losecoin, short averageNum = 1);
	int gameOver();

	//设置下一轮的信息，并且发牌
	int setNextRound();
	void setKickTimer();
	int setBottomPool(short currRound);
	void setUserBetBool(int index, int64_t betroundcoin);
	int setFirstBetter();

	void setSumPool();
	int64_t getSumPool();

	void calcMaxValue(Player* player);
	bool isAllRobot();
	void privateRoomOp();
//时间函数
public:
	void stopAllTimer();
	void startBetCoinTimer(int uid,int timeout);
	void stopBetCoinTimer();
	void startTableStartTimer(int timeout);
	void stopTableStartTimer();
	void startKickTimer(int timeout);
	void stopKickTimer();
	void startSendCardTimer(int timeout);
	void stopSendCardTimer();
	void startGameOverTimer(int timeout);
	void stopGameOverTimer();
public:
	//新增站起和坐下函数
	int playerSeatInTab(Player* player, int tabindex);
	int playerStandUp(Player* player);
	int playerSeatDown(Player* player, int tabindex);
	int playerChangeSeat(Player* player, int tabindex);
    inline bool isViewers(Player *player) { return m_Viewers.find(player) != m_Viewers.end(); }
    void LeaveViewers(Player* player);
    void JoinViewers(Player* player);
	void SendGift(Player* player, int price, BYTE sendtoall);
//游戏逻辑相关
public:
	GameLogic m_GameLogic;					//游戏逻辑
	BYTE m_cbCenterCardData[MAX_CENTERCOUNT];

	//新增站起和坐下
	PlayerSet m_Viewers;
public:
	int id;
	short m_nStatus;						//-1不可用 0 空

	Player* player_array[GAME_PLAYER];

	bool isthrowwin;
	bool hassendcard;

	int m_nType;							//金币场类型
	int64_t m_lBigBlind;					//大盲
	float m_lTax;							//税收
	int64_t m_lGameLimt;					//这局能够下注的最大额度
	int m_nMustBetCoin;						//必下的金币额度
	BYTE m_bCurrRound;
	int64_t m_lCurrMax;						//当前此轮下注最大的金额
	int m_nRaseUid;							//加注的用户
	int m_nCurrBetUid;						//当前下注的人
	int64_t m_lSumPool;						//底池总共数额
	//中途玩牌下注了的人离开的信息
	LeaverInfo leaverarry[GAME_PLAYER];
	short leavercount;
	short m_nCountPlayer;
	int m_nMagicCoin;						//魔法表情金币
public:
	BYTE m_bMaxNum;							//当前桌子最大人数
	Player* m_pFirstBetter;					//第一个下注的人
	short m_nDealerIndex;					//dealer位置
	int m_nDealerUid;						//dealer用户ID
	short m_nSmallBlindIndex;				//小盲的位置
	int m_nSmallBlindUid;					//小盲的用户ID
	short m_nBigBlindIndex;					//大盲的位置
	int m_nBigBlindUid;						//大盲的用户ID
	int64_t m_lCanMaxBetCoin;
	PoolInfo m_PoolArray[GAME_PLAYER];		//边池的数组
	short m_nPoolNum;						//边池的个数
	bool m_bThisRoundHasAllin;				//这轮是否有人allin的标志
	bool m_bIsOverTimer;					//是否启动结束超时时间
	bool m_bIsFirstSetPool;					//大小盲之后就有人allin了

//私人房信息
public:
	int m_nOwner;
	bool m_bHaspasswd;
	char tableName[64];
	char password[64];

//准备信息，处理第一次进入要倒计时准备的问题
public:
	//当前桌子是否是在踢人倒计时
	bool kicktimeflag;
	time_t KickTime;
//用于进入机器人
public:
	time_t m_nRePlayTime;

	time_t SendCardTime;
private:
	TableTimer timer ;

	time_t StartTime;							//开始玩牌的时间
	time_t EndTime;								//结束玩牌的时间
	short EndType;

	char GameID[64];
};

#endif

