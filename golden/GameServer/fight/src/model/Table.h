#ifndef _TABLE_H
#define _TABLE_H

#include <time.h>
#include <vector>
#include "TableTimer.h"
#include "Player.h"
#include "GameLogic.h"
#include "Configure.h"

const int GAME_PLAYER = 5;			//5人金花

#define STATUS_TABLE_CLOSE		-1	//桌子关闭
#define STATUS_TABLE_EMPTY		0	//桌子空
#define STATUS_TABLE_READY		1	//桌子正在准备
#define STATUS_TABLE_ACTIVE		2	//桌子正在玩牌
#define STATUS_TABLE_OVER		3	//桌子游戏结束

#define GAME_OVER_TIME		1500
#define COMPARE_OVER_TIME	5000

#define COMPARE_NOTIFY_DELAY_TIME 5500

typedef struct _LeaverInfo
{
	int uid;
	short m_nTabIndex;
	char name[64];
	int64_t m_lSumBetCoin;
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
	int m_nUseMulCoin;				//用户翻倍使用的金币
	int m_nUseForbidCoin;			//用户禁比使用的金币
	int m_nUseChangeCoin;			//用户换牌使用的金币
}LeaverInfo;


typedef struct _CompareResultInfo
{
    Player* p1;    // 请求比牌的玩家
    Player* p2;    // 被比牌的玩家
    int result; // 0, id1 比牌胜， 1， id2 比牌胜 
    int64_t cmpMoney;
}CompareResultInfo;

class Table
{
	public:
		Table();
		Table(int _id);
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
		int playerComming(Player* player);
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
		void gameStart();
		void setNextFirstPlayer(Player* player);
		//void setAllinCoin();
		int PlayerChangeCard(Player* player, BYTE CardValue);
		void playerBetCoin(Player* player, int64_t betmoney);
		int compareCard(Player* player, Player* opponent);
		bool iscanGameOver();
		bool hasSomeOneAllin();
		bool hasSomeOneForbid();
		int calcCoin();
		int gameOver(bool compareflag = false);

		void setKickTimer();
		void calcMaxValue(Player* player);
		bool isAllRobot();

		bool CheckIP(std::string strip);

		void NotifyUnknowCardMaxNum(Player* player);
		int64_t FormatValue(BYTE fromType, int64_t fromValue, BYTE toType);

        Player* getOtherPlayer(Player* curPlayer);
        int getActivePlayingPlayerCount();
        bool setCompareResultInfo(int result , Player* p1 , Player* p2, int64_t cmpMoney);
        void ClearCompareResultInfo();
		bool isLoseWithin3Day(Player* pl);

	//时间函数
	public:	
		void stopAllTimer();
		void startBetCoinTimer(int uid,int timeout);
		void stopBetCoinTimer();
		void startTableStartTimer(int timeout);
		void stopTableStartTimer();
		void startKickTimer(int timeout);
		void stopKickTimer();
		void startGameOverTimer(int timeout);
		void stopGameOverTimer();
        void startCompareResultNotifyTimer(int timeout);
        void stopCompareResultNotifyTimer();
		
	//游戏逻辑相关
	public:
		GameLogic m_GameLogic;								//游戏逻辑
		BYTE m_bTableCardArray[GAME_PLAYER][MAX_COUNT];		//桌面扑克
	public:
		int id;
		short m_nStatus;						//-1不可用 0 空 
		
		Player* player_array[GAME_PLAYER];
        std::string player_statistic_info_[GAME_PLAYER];

		bool isthrowwin;
		bool hassendcard;

		int m_nType;							//金币场类型
		BYTE m_bCurrRound;
		int m_nRaseUid;							//加注的用户
		int64_t m_lCurrBetMax;					//当前最大的额度是
		BYTE m_BetMaxCardStatus;				//最大下注时的手牌状态
		int m_nCurrBetUid;						//当前下注的人
		short m_nFirstIndex;					//当前这局开始叫牌的位置
		short m_nDealerIndex;					//第一个发牌的位置
		short m_nPreWinIndex;					//上局赢取的桌子位置
		BYTE m_AllinCardStatus;					//全下的状态
		//中途玩牌下注了的人离开的信息
		LeaverInfo leaverarry[GAME_PLAYER];
		short leavercount;
		short m_nCountPlayer;
		int m_nAnte;
		int m_nTax;
		int m_nRase1;
		int m_nRase2;
		int m_nRase3;
		int m_nRase4;
		int m_nMaxLimit;						//单注最大上限
		int64_t m_lMaxAllIn;					//allin最大金币数额
		int64_t m_lHasAllInCoin;				//已经全下过了的金币数，用于如果有人全下则直接这个值为全下额度
		int m_nMagicCoin;						//魔法表情金币
		int64_t m_lSumPool;						//池子总数
		int64_t m_lLeaveSumCoin;				//中途离开者的下注总数（用于相同大小牌型的人数不唯一的结算）
		std::vector<int64_t> m_vecBetCoin;
		bool m_bSetAllinCoinFlag;				//是否设置用户全下操作可点的标志
	public:
		BYTE m_bMaxNum;							//当前桌子最大人数
		int64_t m_lCanMaxBetCoin;
		bool m_bIsOverTimer;					//是否启动结束超时时间

	//娱乐玩法
	public:
		short m_nMulCount;
		short m_nMulNum1;
		int m_nMulCoin1;
		short m_nMulNum2;
		int m_nMulCoin2;

		short m_nForbidCount;
		short m_nForbidNum1;
		int m_nForbidCoin1;
		short m_nForbidNum2;
		int m_nForbidCoin2;

		short m_nChangeCount;
		int m_nChangeCoin1;
		int m_nChangeCoin2;
		int m_nChangeCoin3;

	//私人房信息
	public:
		int m_nOwner;
		bool m_bHaspasswd;
		char tableName[64];
		char password[64];
		short m_bPrivateMaxNum;

	//准备信息，处理第一次进入要倒计时准备的问题
	public:
		//当前桌子是否是在踢人倒计时
		bool kicktimeflag;
		time_t KickTime;
	//用于进入机器人
	public:
		time_t m_nRePlayTime;

		time_t SendCardTime;

        CompareResultInfo CmpRInfo;                 // 未通知客户端的比牌结果，延迟发送，让客户端可以显示两个完整的比牌效果

	private:
		TableTimer timer ;

		time_t StartTime;							//开始玩牌的时间
		time_t EndTime;								//结束玩牌的时间
		short EndType;

		char GameID[64];
};

#endif

