#ifndef _TABLE_H
#define _TABLE_H

#include <time.h>
#include <list>
#include <vector>
#include "Player.h"
#include "GameLogic.h"
#include "Configure.h"
#include "TableTimer.h"

const int GAME_PLAYER = 512;			//

//历史记录
#define MAX_SCORE_HISTORY			15									//历史个数

#define STATUS_TABLE_CLOSE	-1 //桌子关闭
#define STATUS_TABLE_EMPTY	 0
#define STATUS_TABLE_IDLE	 1 //桌子空闲
#define STATUS_TABLE_BET	 2 //桌子正在下注
#define STATUS_TABLE_OPEN    3	//桌子正在开牌

const int MAX_MUL = 10;			//最大倍数

const int MAX_SEAT_MUL = 10;    //上庄倍数

const int MAX_SEAT_NUM = 6;

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
	int64_t jackpotswitch;		//触发分奖池给机器人的额度
	int  tax;
	_Cfg():type(0), bankerwincount(0), betwincount(0),starthour(0),startmin(0),endhour(0),endmin(0),
		limitcoin(100000),retaincoin(10000),upperlimit(90000000),lowerlimit(-900000),bankerupperlimit(90000000),
		bankerlowerlimit(-900000),jackpotswitch(50000000)
	{}
}Cfg;



typedef struct _BetInfo
{
	int nUid;
	short nSid;
	BYTE bType;
	int nBetCoin;
}BetInfo;

//记录信息
typedef struct _ServerGameRecord
{
	BYTE							cbBanker;						//荘家
	BYTE							cbTian;							//天
	BYTE							cbDi;							//地
	BYTE							cbXuan;							//玄
	BYTE							cbHuang;						//黄
}ServerGameRecord;

typedef struct _JackPotPlayer
{
	int id;
	char name[64];
	char json[1024];
	int64_t m_lRewardCoin;
}JackPotPlayer;

class Table
{
	public:
		Table();
		virtual ~Table();
		
		void init();
		void reset();
	//内置函数
	public:
		inline bool isClose(){return m_nStatus==STATUS_TABLE_CLOSE;};
		inline bool isEmpty(){return m_nStatus==STATUS_TABLE_EMPTY || m_nCountPlayer == 0;};
		inline bool isNotFull() {return m_nCountPlayer < Configure::getInstance().numplayer;};
		inline bool isIdle() {return this->m_nStatus == STATUS_TABLE_IDLE;};
		inline bool isBet() {return this->m_nStatus == STATUS_TABLE_BET;};
		inline bool isOpen() {return this->m_nStatus == STATUS_TABLE_OPEN;};
		inline void setStatusTime(time_t t) {statusSwitchTime = t;};
		inline time_t getStatusTime() {return this->statusSwitchTime;};

		inline void setGameID(const char* id) {strcpy(GameID, id);}
        inline const char* getGameID() {return GameID;}
		void setStartTime(time_t t);
		inline time_t getStartTime(){return StartTime;}
		inline void setEndTime(time_t t){EndTime = t;}
		inline time_t getEndTime(){return EndTime;}
	//行为函数
	public:
		Player* isUserInTab(int uid);
		Player* getPlayer(int uid);
		void setToPlayerList(Player* player);
		int playerComming(Player* player);
		int playerBetCoin(Player* player, short bettype, int64_t betmoney);
		void playerLeave(int uid);
		void playerLeave(Player* player);
		void setSeatNULL(Player* player);
		int GetSpecialType();
		bool SetResult(int64_t bankerwincount, int64_t userbankerwincount);
		bool SysSetResult(short result);
		bool SysSetJackPotResult(short jackpotindex, short jackcardtype);
		bool SetLoserResult(Player* loser);
		void GameOver();
		void CalculateScore();
		void pushJackPoter(Player* player);
		void clearJackPoter();
		bool isAllRobot();

		void setToBankerList(Player* player);
		void CancleBanker(Player* player);
		void rotateBanker();
		void HandleBankeList();
		void setBetLimit(Player* banker);

		Player* getLoseUser();

		bool EnterSeat(int seatid, Player *player);
		bool LeaveSeat(int seatid);
		void SendSeatInfo(Player *player);
		void NotifyPlayerSeatInfo();
		void SaveBetInfoToRedis(Player * player, int bettype);
		void ClearBetInfo();

		void ReceivePushCardType(const std::vector<BYTE> cardtypes);

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
		short m_nRecordLast;
		ServerGameRecord m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
		GameLogic m_GameLogic;									//游戏逻辑
		BYTE m_bTableCardArray[5][MAX_COUNT];					//桌面扑克
		CardTypeMul m_CardResult[5];							//记录桌子5个区域牌的牌型和倍数

	public:
		int id;
		short m_nType;
		short m_nStatus;					//-1不可用 0 空 
		short m_nCountPlayer;
		int m_nRePlayTime;
		bool m_bHasBet;						//这段时间是否有下注，方便后面是否500毫秒发给所有用户
		int64_t m_lTabBetArray[BETNUM];
		int64_t m_lTabBetLimit;	//各区域下注限制，根据庄家当前金币计算
		int64_t m_lRealBetArray[BETNUM];	//真实下注的数额只包含真实用户
		int m_nLimitCoin;
		int m_nRetainCoin;					//输到还要剩余多少金币
		int m_nDespatchNum;
		int m_nTax;							//台费率
			
		//用户排序列表
		std::list<Player*> PlayerList;

		//申请上庄列表
		std::list<Player*> BankerList;

		//获得奖励的列表
		std::list<JackPotPlayer*> JackPotList;
		BYTE m_bJackPotCard[3];
		BYTE m_bJackCardType;
		time_t m_tRewardTime;
		int64_t m_lJackPot;				//奖池
		int64_t m_lGivePlayerJackPot;	//给用户的奖池总额
		int64_t m_lGetJackPotArea[BETNUM];
		
		Player* player_array[GAME_PLAYER];
		Player* maxwinner;

	//做庄的信息
	public:
		short bankersid;
		int bankeruid;//做庄的uid
		short bankernum;//连续做庄的次数
		int64_t m_lBankerWinScoreCount;
	public:
		std::vector<BetInfo> betVector;


		Player*	m_SeatidArray[MAX_SEAT_NUM];

	public:
		int m_nOneChatCost;		//一次聊天花费
		int m_nLimitChatCoin;	//聊天限制金币
		vector<int> m_vecBlacklist;		//黑名单用户
	public:
		//当前桌子是否是在踢人倒计时
		time_t StartTime;//开始玩牌的时间
		time_t EndTime;//结束玩牌的时间
	private:
		TableTimer timer ;
		time_t statusSwitchTime;

		std::vector<BYTE> receive_push_card_type; //收到php推送的牌型

		char GameID[80];
};

#endif

