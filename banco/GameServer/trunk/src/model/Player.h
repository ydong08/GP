#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <time.h>
#include <stdint.h>
#include <Typedef.h>
using namespace std;

#define STATUS_PLAYER_LOGOUT	0
#define STATUS_PLAYER_INGAME	1  //正在打牌
#define STATUS_PLAYER_COMING	2  //入座
#define STATUS_PLAYER_RSTART	3  //入座，准备开始
#define STATUS_PLAYER_ACTIVE	4  //正在打牌
#define STATUS_PLAYER_OVER		5  //游戏结束
#define STATUS_PLAYER_STANDUP   6  //站起
#define STATUS_PLAYER_BANKING   7  //做庄中

#define BETNUM                  6  //下注区域总数

class Player
{
	public:
		Player(){};
		virtual ~Player(){};
		void init();
	//内置函数
	public:
		inline bool isLogout() {return m_nStatus == STATUS_PLAYER_LOGOUT;};
		inline bool isActive() { return m_nStatus == STATUS_PLAYER_ACTIVE; }
		inline bool isReady() { return m_nStatus == STATUS_PLAYER_RSTART; }
		inline void setActiveTime(time_t t){active_time = t;}
        inline time_t getActiveTime(){return active_time;}
		inline void setEnterTime(time_t t){enter_time = t;}
		inline time_t getEnterTime(){return enter_time;}
	//行为函数
	public:
		void leave(bool isSendToUser = true);
		void enter();
		void reset();
		bool notBetCoin();
		void login();
		void standup();
		void sitdown();

	public:
		int id;
		char name[64];
		char json[1024];
		char headlink[128];
		short m_nHallid;
		short m_nStatus;

		int tax;
		int64_t m_lMoney;
		int64_t m_lWinScore;						//最终输赢金币
		int64_t m_lReturnScore;						//最终输赢金币
		int64_t m_lLostScore;						//输金币
		int64_t m_lTempScore;						//暂时
		int tid;
		short source;
		short m_nSeatID;
		int m_nWin;
		int m_nLose;
		int m_nRunAway;
		int m_nTie;
		BYTE m_bisCall;								//这盘是否抢了庄
		int64_t m_lBetArray[BETNUM];
		bool isonline;
		int m_nRoll;
		int m_nExp;
		bool m_bhasOpen;
		bool isCanlcebanker;

		int m_nChatCostCount;						//聊天花费金币
		int m_nLastBetTime;							//最后一次下注的时间

	public:
		short pid;
		short cid;
		short sid;
		short gid;
	private:
		time_t active_time;							//客户端最近活跃时间
		time_t enter_time;							//进入游戏的时间
		time_t replay_time;							//游戏等待时间，新一盘游戏开始时计时，用于机器人判断当前等待时间
		time_t timeout_time;						//游戏给他设置超时时间的时刻

};
#endif
