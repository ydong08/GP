#ifndef _TABLE_H
#define _TABLE_H

#include <time.h>
#include "TableTimer.h"
#include "Player.h"
#include "GameLogic.h"
#include "Configure.h"

const int GAME_PLAYER = 4;			//四人梭哈

#define STATUS_TABLE_CLOSE	-1 //桌子关闭
#define STATUS_TABLE_EMPTY	 0 //桌子空
#define STATUS_TABLE_READY	 1 //桌子正在准备

#define STATUS_TABLE_ACTIVE 2	//桌子正在玩牌
#define STATUS_TABLE_OVER	 3	//桌子游戏结束

typedef struct _LeaverInfo
{
	int uid;
	short tab_index;
	char name[64];
	int64_t betcoin;
	short source;
	short cid;
	short pid;
	short sid;
	int64_t money;
	int nwin;
	int nlose;
	int nrunaway;
	int ntie;
	int m_nMagicNum;
	int m_nMagicCoinCount;
}LeaverInfo;


class Table
{
	public:
		Table();
		virtual ~Table();
		
		void init();
		void reset();
	//内置函数
	public:
		inline void closeTable(){this->status == STATUS_TABLE_CLOSE;};
		inline bool isClose(){return status==STATUS_TABLE_CLOSE;};
		inline bool hasOnePlayer() {return countPlayer == 1;};
		inline bool isEmpty(){return status==STATUS_TABLE_EMPTY || countPlayer == 0;};
		inline bool isNotFull() {return countPlayer < Configure::getInstance()->numplayer;};
		inline bool isActive() {return this->status == STATUS_TABLE_ACTIVE;};
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
		bool isUserInTab(int uid);
		Player* getPlayer(int uid);
		int playerComming(Player* player);
		void playerLeave(int uid);
		void playerLeave(Player* player);
		void setSeatNULL(Player* player);
		//用户在桌子上找到一个座位
		int playerSeatInTab(Player* player);
		bool isCanGameStart();
		bool isAllReady();
		void gameStart();
		Player* getNextBetPlayer(Player* player, short playeroptype);
		int gameOver(Player* winner, bool bisthrowwin = false);
		//计算金币的加减
		int calcCoin(Player *gamewinner);
		void reSetInfo();
		void setTableRent();
		void setRoundTaskConf();
		//void setPlayerCarryCoin();
		//设置下一轮的信息，并且发牌
		int setNextRound();
		bool iscanGameOver(Player **winner);
		bool iscanGameOver();
		//获取能够下注的用户总数，条件就是没有allin并且用手牌
		short getCanPlayNum(); 
		//设置每轮的下注上限
		void setMaxBetCoin();
		Player* getCurrWinner();
		Player* getPreWinner();
		Player* getFirstBetPlayer(Player* winner);
		int64_t getSecondLargeCoin();	
		void calcWinnerOrder();
		void setPlayerOptype(Player* player, short playeroptype);
		bool hasSomeOneAllin();
		void setPlayerlimitcoin(Player* nextplayer);
		void setKickTimer();
		bool isAllRobot();
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
		
	//游戏逻辑相关
	public:
		GameLogic m_GameLogic; //游戏逻辑
		BYTE m_bTableCardArray[GAME_PLAYER][5];		//桌面扑克
		//推断胜者
		Player* deduceWiner(BYTE bBeginPos, BYTE bEndPos);
		Player* deduceWinerOrder(BYTE bBeginPos, BYTE bEndPos);
		short GetPlayerCardKind(BYTE bPlayerCardList[], BYTE bBeginPos, BYTE bEndPos);

	public:
		int id;
		short status;	//-1不可用 0 空 
		
		Player* player_array[GAME_PLAYER];
		Player* pregame_winner;//上一盘赢的用户

		bool isthrowwin;

		int clevel;        //金币场类型
		int64_t ante;        //底注
		int64_t tax;		//税收
		short raterent;	//税收的比率
		short countPlayer; //当前桌子用户数量
		int commonCoin; //充公的金币数，当有用户玩牌期间逃跑
		short currRound; //当前处于下注的第几轮
		int64_t currMaxCoin; //当前此轮下注最大的金额
		int raseUid;	//加注的用户
		int betUid;	//当前下注的人
		int64_t maxbetcoin; //当前这轮金币下注上限
		int64_t thisGameLimit;//当前这局牌上限
		//中途玩牌下注了的人离开的信息
		LeaverInfo leaverarry[GAME_PLAYER];
		short leavercount;

		int winnerorder[GAME_PLAYER];
		short winnercount;
		//房间人数限制
		int maxCount;

	public:
		short m_nRoundNum1;
		short m_nRoundNum2;
		short m_nRoundNum3;
		int m_nGetCoinHigh1;
		int m_nGetCoinLow1;
		int m_nGetCoinHigh2;
		int m_nGetCoinLow2;
		int m_nGetCoinHigh3;
		int m_nGetCoinLow3;
		BYTE m_bRewardType;
		int m_nGetRollHigh1;
		int m_nGetRollLow1;
		int m_nGetRollHigh2;
		int m_nGetRollLow2;
		int m_nGetRollHigh3;
		int m_nGetRollLow3;
		int m_nMagicCoin;
	//私人房信息
	public:
		int owner;
		bool haspasswd;
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
		bool hassendcard;
	private:
		TableTimer timer ;

		time_t StartTime;//开始玩牌的时间
		time_t EndTime;//结束玩牌的时间
		short EndType;

		char GameID[64];

};

#endif

