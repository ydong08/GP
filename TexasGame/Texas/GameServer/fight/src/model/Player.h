#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <time.h>
#include <stdint.h>
#include <set>
#include "Typedef.h"
#include "TaskManager.h"
#include "GameLogic.h"

using namespace std;

#define STATUS_PLAYER_LOGOUT 0
#define STATUS_PLAYER_LOGIN 1   //登陆，未入座
#define STATUS_PLAYER_COMING 2  //入座
#define STATUS_PLAYER_RSTART 3  //入座，准备开始
#define STATUS_PLAYER_ACTIVE 4  //正在打牌
#define STATUS_PLAYER_OVER 5    //游戏结束


#define OP_CALL	 1	//跟注
#define OP_RASE  2	//加注
#define OP_ALLIN 4	//梭哈
#define OP_CHECK 8	//看牌
#define OP_THROW 16	//放弃

#define  GAME_END_NORMAL       0
#define  GAME_END_DISCONNECT   1

typedef struct _CoinCfg
{
	short level;		//等级
	int maxmoney;		//进入的最大金币
	int minmoney;		//进入的最小金币
	int tax;
	int retaincoin;		//保留金币，大场里面留下的金币
	int carrycoin;		//带入金币数，在小场
	int carrycoinmax;	//可以再此买入的金币数额
	int bigblind;		//大盲多大
	int maxnum;		//最大人数
	int magiccoin;		//魔法表情金币
	int rateante;		//按最小带入金币的百分几大盲
	int ratetax;		//固定比率按低注的百分之几算台费
	int mustbetcoin;	//一定得下注的额度
	_CoinCfg():level(0),maxmoney(0),minmoney(0),tax(1),retaincoin(0),carrycoin(0),
		carrycoinmax(800),bigblind(10),maxnum(6),magiccoin(10), rateante(10), ratetax(100), mustbetcoin(0)
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
		void setCarryMoney();
		int setCarryInNextRound(int64_t money);

		void calcMaxWin();


	public:
		int id;								//用户ID
		char name[64];
		short level;						//等级
		short m_nSeatID;					//座位号
		short tid;							//当前属于哪个桌子ID
		short m_nTabIndex;					//当前在桌子的位置
		int64_t m_lMoney;					//金币数
		int64_t m_lCarryMoney;				//这盘牌携带金币数
		int m_nWin;							//赢牌数
		int m_nLose;						//输牌数
		int m_nRunAway;						//逃跑的次数
		int	m_nTax;							//台费
		int m_nTie;
		char json[1024];					//json字符串
		short m_nStatus;					//状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
		short source;						//用户来源
		int m_nType;						//当前金币场底注 0-无底注
		BYTE card_array[MAX_COUNT];			//牌数
		BYTE cbEndCardData[MAX_CENTERCOUNT];
		bool m_bHasCard;					//是否有手牌
		short m_nHallid;					//当前这个用户在那个大厅ID
		BYTE m_nEndCardType;				//最终手牌类型
		bool m_bThisRoundBet;				//当前这轮是否已经下注
		bool m_bAllin;						//是否已经allin
		short optype;
		int64_t m_lBetLimit;				//下注限制
		bool isorder;						//是否已经有序了
		bool isdropline;					//是否已经掉线
		int m_nRoll;						//兑换券
		short m_nBuyInFlag;					//每局结束买入最大值
		int64_t m_lAddMoney;				//新局重新增加的金币
		BYTE m_bTempCardType;				//零时存放牌型 给任务系统用
	//用于结算的临时数据
	public:
		BYTE cbTempCardDate[MAX_CENTERCOUNT];
		UserWinList tempWinlist;

	public:
		//统计字段
		short cid;
		short sid;
		short pid;
		//超时下注的次数，只是第一轮就超时
		short timeoutCount;

		CoinCfg coincfg;
		//任务信息
	public:
		//牌型任务
		short m_nGetRoll;
		bool m_bCompleteTask;
		Task* m_pTask;
		
		short m_nMagicNum;
		int m_nMagicCoinCount;

		int64_t m_lMaxWinMoney;
		int64_t m_lMaxCardValue;

		//准备信息，处理第一次进入要倒计时准备的问题
	public:
		short startnum;
		//下注信息 四轮的下注信息其中 betCoinList[0] 是此用户的总下注
	public:
		int64_t betCoinList[5];
		//最终获得的金币数，可为正也可为负
		int64_t m_lFinallGetCoin;
		//最终获得的底池个数
		std::set<int> m_setPoolResult;
		int64_t m_PoolNumArray[GAME_PLAYER];
	private:
		time_t active_time;					//客户端最近活跃时间
		time_t enter_time;					//进入游戏的时间
		time_t betcoin_time;				//开始计算用户下注超时的时间
		time_t replay_time;					//游戏等待时间，新一盘游戏开始时计时，用于机器人判断当前等待时间

};
#endif
