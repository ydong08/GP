#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <time.h>
#include <stdint.h>
#include <set>
#include <json/json.h>
#include "Typedef.h"
#include "GameLogic.h"
#include <vector>
#include <set>
using namespace std;

#define STATUS_PLAYER_LOGOUT 0
#define STATUS_PLAYER_LOGIN  1  //登陆，未入座
#define STATUS_PLAYER_COMING 2  //入座
#define STATUS_PLAYER_RSTART 3  //入座，准备开始
#define STATUS_PLAYER_ACTIVE 4  //正在打牌
#define STATUS_PLAYER_OVER   5  //游戏结束

#define  GAME_END_NORMAL       0
#define  GAME_END_DISCONNECT   1
#define  GAME_END_DISCARD     2 

enum CARD_STATUS
{
	CARD_DISCARD = 0,		//比牌输
	CARD_UNKNOWN = 1,		//闷牌
	CARD_KNOWN = 2,			//看牌
	//CARD_LOSE = 3,			//弃牌
};

#define OP_CALL		1	//跟注
#define OP_RASE		2	//加注
#define OP_ALLIN	4	//全下
#define OP_CHECK	8	//看牌
#define OP_THROW	16	//放弃
#define OP_COMPARE	32	//比牌
#define OP_CALLING	64	//控制跟到底

typedef struct _CoinCfg
{
	short level;		//等级
	int maxmoney;		//进入的最大金币
	int minmoney;		//进入的最小金币
	int tax;
	int ante;
	int rase1;			//加注1
	int rase2;			//加注2
	int rase3;			//加注3
	int rase4;			//加注4
	int maxlimit;		//封顶
	int maxallin;
	int magiccoin;		//魔法表情金币
	int check_round;  //第几轮以前不能看牌
	int compare_round; //第几轮可以比牌
	_CoinCfg():level(0),maxmoney(0),minmoney(0),tax(1),ante(10),rase1(0),rase2(0),rase3(0),rase4(0),
		maxlimit(1000), maxallin(10000), magiccoin(10), check_round(1), compare_round(2)
	{}
}CoinCfg;

class Player
{
	public:
		Player(){};
		virtual ~Player(){};
		void init();
		void reset();

	//内置函数
	public:
		inline bool isLogout(){ return m_nStatus == STATUS_PLAYER_LOGOUT; }
		inline bool isLogin(){ return m_nStatus == STATUS_PLAYER_LOGIN; }
		inline bool isComming(){ return m_nStatus == STATUS_PLAYER_COMING; }
		inline bool isReady(){ return m_nStatus == STATUS_PLAYER_RSTART; }
		inline bool isActive(){ return m_nStatus == STATUS_PLAYER_ACTIVE; }
		inline bool isGameOver(){ return m_nStatus == STATUS_PLAYER_OVER; }
		inline void setActiveTime(time_t t){active_time = t;}
        inline time_t getActiveTime(){return active_time;}
		inline void setEnterTime(time_t t){enter_time = t;}
		inline time_t getEnterTime(){return enter_time;}
		inline void setBetCoinTime(time_t t){betcoin_time = t;}
		inline time_t GetBetCoinTime(){return betcoin_time;}
		inline void setRePlayeTime(int t){replay_time = t;}
		inline int getRePlayeTime(){return replay_time;}
	//行为函数
	public:
		void login();
		void leave();
		void enter();

		bool checkMoney();
		bool hasUseCoin();
		void calcMaxWin();

		static int64_t calculateCompMoney(Player* player, BYTE betmaxcard, int64_t betmax, int maxlimit);

        void recordCard(std::string & cards);
        void recordThrowCard(int round , int64_t sumBetCoin);

        void recordCompareCard(int round, int otherUid, int64_t compareMoney, int result);


        void recordActiveLeave(int round);
        void recordRaise(int round , int64_t raiseMoney);
        void recordCall(int round , int64_t callMoney);
        void recordCheck(int round , int64_t checkMoney);
        void recordAllIn(int round , int64_t allInMoney);
        void recordWin(int64_t winMoney);

        Json::Value& gameRecordAnalysisInfo();
        void clearRecordAnalysisInfo()
        {
            game_play_record.clear();
        }

		// 设置最终获得的金币数
		// 参数：金币的变化数 (正数-增加; 负数-减少)
		void setFinalGetCoin(int64_t delta_coin);

	public:
		int id;								//用户ID
		char name[64];
		short level;						//等级
		short m_nSeatID;					//座位号
		short tid;							//当前属于哪个桌子ID
		short m_nTabIndex;					//当前在桌子的位置
		int64_t m_lMoney;					//金币数
		int m_nWin;							//赢牌次数
		int m_nLose;						//输牌次数
		int m_nRunAway;						//逃跑的次数
		int m_nTie;
		int m_nRoll;						//兑换券
		char json[1024];					//json字符串
		short m_nStatus;					//状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
		short source;						//用户来源
		int m_nType;						//当前金币场底注 0-无底注
		BYTE card_array[MAX_COUNT];			//牌数
		BYTE m_bCardStatus;					//手牌状态 0弃牌 1有但是没有看盘 2有但是看牌了 3表示比牌输
		short m_nHallid;					//当前这个用户在那个大厅ID
		BYTE m_nEndCardType;				//最终手牌类型
		short optype;
		bool isdropline;					//是否已经掉线
        bool is_discard;                    //是否弃牌
		bool m_bAllin;
		int64_t m_lSumBetCoin;
		std::set<int> m_setCompare;			//比过牌的玩家列表
		int64_t m_AllinCoin;
		bool m_bCompare;					//是否主动比牌
		
		char m_byIP[32];                    //IP
		int64_t m_lTotalMoney;					//入场金币数+保险箱内金币
	public:
		//统计字段
		short cid;
		short sid;
		short pid;
		//超时下注的次数，只是第一轮就超时
		short timeoutCount;

		bool isbacklist;
        Json::Value game_play_record;
		//任务信息
	public:
		//牌型任务
		short m_nGetRoll;
		bool m_bCompleteTask;
		/*Task* m_pTask;*/
		
		short m_nMagicNum;
		int m_nMagicCoinCount;

		int64_t m_lMaxWinMoney;		//单次赢得最多金币记录
		int64_t m_lMaxCardValue;
	public:
		short m_nUseMulNum;				//用户选了翻倍
		int m_nUseMulCoin;				//用户翻倍使用的金币
		short m_nUseForbidNum;			//用户选择了禁比轮数
		int m_nUseForbidCoin;			//用户禁比使用的金币
		short m_nChangeNum;				//用户换牌次数
		int m_nUseChangeCoin;			//用户换牌使用的金币

		//准备信息，处理第一次进入要倒计时准备的问题
	public:
		short startnum;
		//下注信息 四轮的下注信息其中 betCoinList[0] 是此用户的总下注
	public:
		//最终获得的金币数，可为正也可为负
		int     m_nTax;
		int64_t m_lFinallGetCoin;
	private:
		time_t active_time;					//客户端最近活跃时间
		time_t enter_time;					//进入游戏的时间
		time_t betcoin_time;				//开始计算用户下注超时的时间
		time_t replay_time;					//游戏等待时间，新一盘游戏开始时计时，用于机器人判断当前等待时间
protected:
	RedisAccess m_Redis;

};

#endif
