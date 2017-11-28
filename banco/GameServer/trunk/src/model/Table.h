#ifndef _TABLE_H
#define _TABLE_H

#include <time.h>
#include <list>
#include <vector>
#include "Player.h"
#include "GameLogic.h"
#include "Configure.h"
#include "TableTimer.h"

const int GAME_PLAYER = 6;			                                    //include robot
const int MAX_SEAT_MUL = 1;   	                                        //上庄倍数
const int MAX_SEAT_NUM = 6;

//机器人庄家信息
const int ROBOT_BANKER_ID = 10000;
const int ROBOT_BANKER_MONEY = 100000;

//历史记录
#define MAX_SCORE_HISTORY			100									//历史个数

#define CHIP_COUNT                  4                                   //筹码类型总数
#define INDEX_PLAYER                0                                   //玩家索引
#define INDEX_BANKER                1                                   //庄家索引

#define SHUFFLE_CRITICAL_COUNT      200                                 //剩余牌数量 (小于该值时需要洗牌)

//桌子状态
#define STATUS_TABLE_EMPTY	        0
#define STATUS_TABLE_READY	        1                                   //桌子正在准备
#define STATUS_TABLE_IDLE	        2 	                                //桌子空闲
#define STATUS_TABLE_BET	        3 	                                //桌子正在下注
#define STATUS_TABLE_OPEN           4	                                //桌子正在开牌
#define STATUS_TABLE_SETTLEMENT     5                                   //桌子正在结算

#define AREA_XIAN_DUI               1                                   //闲对子
#define AREA_PING                   2                                   //平家索引
#define AREA_ZHUANG_DUI             3                                   //庄对子
#define AREA_XIAN                   4                                   //闲家索引
#define AREA_ZHUANG                 5                                   //庄家索引
#define AREA_MAX                    BETNUM                              //最大区域

//赢家类型
#define WIN_TYPE_NONE               0
#define WIN_TYPE_ZHUANG             1
#define WIN_TYPE_HE                 2
#define WIN_TYPE_XIAN               3

//区域倍数multiple
#define MULTIPLE_XIAN				1									//闲家倍数
#define MULTIPLE_PING				8									//平家倍数
#define MULTIPLE_ZHUANG				1									//庄家倍数
#define MULTIPLE_TONG_DIAN			33									//同点平倍数
#define MULTIPLE_XIAN_PING			11									//闲对子倍数
#define MULTIPLE_ZHUANG_PING		11									//庄对子倍数

typedef struct _CardTypeMul
{
	short type;
	short mul;
}CardTypeMul;

typedef struct _Cfg
{
	short type;
	int bankerwincount;			//当莊机器人赢金币数
	int betwincount;			//下注机器人赢金币数
	int limitcoin;				//下注限制
	int retaincoin;
	char starttime[16];			//开始时间
	char endtime[16];			//结束时间
	BYTE starthour;
	BYTE startmin;
	BYTE endhour;
	BYTE endmin;
	int64_t upperlimit;
	int lowerlimit;
	int64_t bankerupperlimit;
	int bankerlowerlimit;
	int tax;                    //税率
	int64_t bankerlimit;        //上庄限制
	int bankernum;              //做庄最大次数
	int betnum;                 //加注上限
    int minbetnum;              //加注下限
    int chiparray[CHIP_COUNT];  //加注的类型列表
	int	PersonLimitCoin;
	int ante;
	_Cfg():type(0), bankerwincount(0), betwincount(0),starthour(0),startmin(0),endhour(0),endmin(0), limitcoin(100000),
		retaincoin(10000),upperlimit(90000000),lowerlimit(-900000),bankerupperlimit(90000000),bankerlowerlimit(-900000), tax(5), bankerlimit(500000), bankernum(5), betnum(20000),
        minbetnum(100), PersonLimitCoin(100000), ante(10)
	{}
}Cfg;

typedef struct _BetInfo
{
	int nUid;
	short nSid;
	BYTE bType;
	int nBetCoin;
}BetInfo;


enum BET_TYPE
{
    BET_NONE = 0,
};

class Table;

class BetArea
{
public:
    BetArea(BET_TYPE bt, Table* tab);
    
    BYTE getGameRecord() const { return gameRecord;}
    BET_TYPE getBetType() const { return betType;}
    int64_t getBetLimit() const { return betLimit;}
    int64_t getBetMoney() const { return betMoney;}
    int64_t getRealMoney() const { return betRealMoney;}
    
    void setGameRecord(BYTE rec) { gameRecord = rec;}
    void updateBetLimit();
    void updateBetMoney(Player* p);
    void updateRealMoney(Player* p);
    
private:
    BYTE gameRecord;
    BET_TYPE betType;
    int64_t betLimit;       //该区域当前下注限制
    int64_t betMoney;       //该区域当前下注金币,包括机器人
    int64_t betRealMoney;   //该区域玩家下注金币，不包括机器人
    Table* table;
};

//记录信息
typedef struct _ServerGameRecord
{
    BYTE cbKingWinner;      //天王赢家
    BYTE bPlayerTwoPair;    //对子标示
    BYTE bBankerTwoPair;    //对子标示
    BYTE cbPlayerCount;     //闲家点数
    BYTE cbBankerCount;     //庄家点数
    BYTE cbWinType;         //赢家类型，庄，闲，和
}ServerGameRecord;

class  ScoreIndexRecord
{
public:
    int64_t score;
    BYTE    index;

public:
    bool operator < (const ScoreIndexRecord &m)const
    {
        return score < m.score;
    }
};

class Table
{
	public:
		Table();
		virtual ~Table();
		
		void init();
		void reset();

	//内置函数
	public:
		inline bool isEmpty(){return m_nStatus==STATUS_TABLE_EMPTY || getPlayerCount(false, true) == 0;};
		inline bool isNotFull() { return getPlayerCount(false) < GAME_PLAYER; };
		inline bool isIdle() {return this->m_nStatus == STATUS_TABLE_IDLE;};
		inline bool isBet() {return this->m_nStatus == STATUS_TABLE_BET;};
		inline bool isOpen() {return this->m_nStatus == STATUS_TABLE_OPEN;};
		inline bool isActive() { return this->m_nStatus >= STATUS_TABLE_IDLE && this->m_nStatus < STATUS_TABLE_OPEN; };
		inline void setStatusTime(time_t t) {statusSwitchTime = t;};
		inline time_t getStatusTime() {return this->statusSwitchTime;};

		void setGameID(char* gameID, Player* banker = NULL);
        inline const char* getGameID() {return GameID;}
		void setStartTime(time_t t);
		inline time_t getStartTime(){return StartTime;}
		inline void setEndTime(time_t t){EndTime = t;}
		inline time_t getEndTime(){return EndTime;}

		inline void lockTable() { kicktimeflag = true; KickTime = time(NULL); }
		inline void unlockTable() { kicktimeflag = false; }
		inline bool isLock() { return kicktimeflag; }
	//行为函数
	public:
		Player* isUserInTab(int uid);
		Player* getPlayer(int uid);
        Player* getBanker() { return getPlayer(bankeruid); }
		int playerComming(Player* player);
		int playerBetCoin(Player* player, short bettype, int64_t betmoney);
		void playerLeave(int uid);
		void playerLeave(Player* player);
		void setSeatNULL(Player* player);

        int64_t RobotBankerGetSystemWin(BYTE openType);
        int64_t PlayerBankerGetSystemWin(BYTE openType);
        void CalcualAllResult();
        void SortOpenTypes();
        BYTE GetSafeOpenType();
        void OpenType2WinAreasInfo(BYTE openType , BYTE &winType , BYTE &zhuang_dui , BYTE &xian_dui);
        bool hasSameCards(BYTE cards1[] , BYTE count1 , BYTE cards2[] , BYTE count2);

		bool SetResult(int64_t bankerwincount, int64_t userbankerwincount);
		void GameOver();
		void GameStart();
		void CalculateScore(short result);
		bool isAllRobot();

		void setToBankerList(Player* player);
		void CancleBanker(Player* player);
		void rotateBanker(Player* specified_player = NULL);
		void HandleBankeList();
		void setBetLimit();

        void reloadCfg();
	//时间函数
	public:	
		void startIdleTimer(int timeout);
		void stopIdleTimer();
		void startBetTimer(int timeout);
		void stopBetTimer();
		void startOpenTimer(int timeout);
		void stopOpenTimer();
	//游戏逻辑相关
	public:
        bool EnterSeat(int seatid, Player *player);
        bool LeaveSeat(int seatid);
        void SendSeatInfo(Player *player);
        void NotifyPlayerSeatInfo();
		void SaveBetInfoToRedis(Player * player, int bettype);
		void ClearBetInfo();
        BYTE RandCardByWinType(int randSeed);
        BYTE RandCardByAssignedType(int randSeed, BYTE type);
        void DisplayPlayerBetInfo();
        void DisplayCards();
		bool isAllReady();

        void DispatchTableCard(int randSeed , BYTE type = 0 , BYTE zhuang_dui = 0 , BYTE xian_dui = 0);
        void DeduceWinner(BYTE* pWinArea);
		//用户在桌子上找到一个座位
		int playerSeatInTab(Player* complayer);
		int playerStandUp(Player* player);
		int playerSeatDown(Player* player, int tabindex, bool isChangeSeat = false);
		int playerChangeSeat(Player* player, int tabindex);
		void ConfirmBanker();
		void calculateBetAreaLimit(Player* banker);
		BYTE getWinType(BYTE cbWinArea[AREA_MAX]);
		short getPlayerCount(bool canPlayOnly, bool exceptRobot = false) const;
		bool isWaitForBanker(int uid) const;
	//php推送
	public:
		void receivePush(int winarea, BYTE zhuangdui, BYTE xiandui);
		void phpRandCard(int randSeed);	//收到php推送后抽牌

	public:
		int id;
		short m_nStatus;											//-1不可用 0 空 
		bool m_bHasBet;												//这段时间是否有下注，方便后面是否500毫秒发给所有用户
		int64_t m_lTabBetArray[BETNUM];
		int64_t m_lRealBetArray[BETNUM];							//真实下注的数额只包含真实用户
		int m_nLimitCoin;											//最低下注限制
		int m_nPersonLimitCoin;										//个人下注限制
		int m_nRetainCoin;											//输到还要剩余多少金币
        int m_nChipArray[CHIP_COUNT];								//筹码数组

		short m_nRecordLast;
		ServerGameRecord m_GameRecordArrary[MAX_SCORE_HISTORY];		//游戏记录
		int m_historyCount;

		CGameLogic m_GameLogic;										//游戏逻辑
		BYTE m_cbCardCount[2];										//扑克数目
		BYTE m_cbTableCardArray[2][3];
        int m_RemainCardCount;                                      //洗牌后剩余扑克总数

        BYTE m_bWinType;
        BYTE m_cbBankerPoint;
        BYTE m_cbPlayerPoint;
        BYTE m_cbBankerPair;
        BYTE m_cbPlayerPair;
    
		int m_nDespatchNum;
		int m_nTax;
		int64_t m_nBankerlimit;										//上庄限制
		int m_nMaxBetNum;											//房间加注最大值
        int m_nMinBetNum;											//房间加注最小值
		
		Player* player_array[GAME_PLAYER];							// 玩家列表

		Player* maxwinner;
        Player* m_SeatidArray[MAX_SEAT_NUM];						// 保存座位列表信息用

        int64_t AllOpenResult[12];
        std::vector<ScoreIndexRecord> SortedScordIndex;

		bool kicktimeflag;
		time_t KickTime;
	//做庄的信息
	public:
		Player m_robotbanker;										//系统坐庄
		std::list<Player*> BankerList;								//申请上庄列表
		int bankeruid;												//做庄的uid
		short bankernum;											//连续做庄的次数
		int m_nMaxBankerNum;										//最大连庄次数
		int64_t m_lBankerWinScoreCount;
        bool m_NeedMakeRobotBankerWin;                              //是否要确认机器人庄家赢
	//下注限制
	public:
		std::vector<BetInfo> betVector;
		int64_t betAreaLimit[BETNUM];                               //下注的上限 (表示一个区间: 随着当局游戏内其他玩家下注金额的变化而变化; 庄家的总资产也会影响它;)
		static Cfg coincfg;
	public:
		time_t StartTime;											//开始玩牌的时间
		time_t EndTime;												//结束玩牌的时间
		time_t timeout_time;

	private:
		TableTimer timer ;
		time_t statusSwitchTime;
		char GameID[80];
		bool receivePhpPush;
		BYTE winType;
		BYTE zhuangDui;
		BYTE xianDui;
        int m_nZhuangWinRate;
        int m_nXianWinRate;
        int m_nPingRate;
        int m_nZhuangDuiRate;
        int m_nXianDuiRate;
};

#endif

