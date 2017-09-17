#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <time.h>
#include <stdint.h>
#include "wtypedef.h"
#include "TaskManager.h"

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

typedef struct _CoinCfg
{
	short level; //等级
	int maxmoney; //进入的最大金币
	int minmoney; //进入的最小金币
	short raterent;	//固定比率按低注的百分之几算
	int retaincoin; //保留金币，大场里面留下的金币
	int carrycoin; //带入金币数，在小场
	//完成多少局完成新手任务,初级场、中级场和高级场
	short playCount1;
	short playCount2;
	short playCount3;
	//完成新手任务得到的劵
	short getingot1;
	short getingot2;
	short getingot3;
	int roundnum;	//局数任务各个场次完成获得宝箱
	int coinhigh1;	//第一获取最大金币数额
	int coinlow1;	//第一获取最小金币数额
	int coinhigh2;	//第二获取最大金币数额
	int coinlow2;	//第二获取最小金币数额
	int coinhigh3;	//第三获取最大金币数额
	int coinlow3;	//第三获取最小金币数额
	BYTE rewardtype;//局数任务奖励类型
	int ante;		//底注
	int rollhigh1;	//第一阶段局数任务获取最大金币数额
	int rolllow1;	//第一阶段局数任务获取最小金币数额
	int rollhigh2;	//第二阶段局数任务获取最大金币数额
	int rolllow2;	//第二阶段局数任务获取最小金币数额
	int rollhigh3;	//第三阶段局数任务获取最大金币数额
	int rolllow3;	//第三阶段局数任务获取最小金币数额
	int magiccoin;	//魔法表情扣除金币
	int tax;		//税率
	_CoinCfg():level(0), maxmoney(0), minmoney(0), raterent(0),retaincoin(0),carrycoin(0), playCount1(30), 
		playCount2(30),playCount3(30),getingot1(10),getingot2(10),getingot3(10),roundnum(0),
		coinhigh1(0),coinlow1(0),coinhigh2(0),coinlow2(0),coinhigh3(0),coinlow3(0),rewardtype(1), ante(1),
		rollhigh1(0), rolllow1(0),rollhigh2(0),rolllow2(0),rollhigh3(0),rolllow3(0), magiccoin(50), tax(5)
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
		inline bool isLogout(){ return status == STATUS_PLAYER_LOGOUT; }
		inline bool isLogin(){ return status == STATUS_PLAYER_LOGIN; }
		inline bool isComming(){ return status == STATUS_PLAYER_COMING; }
		inline bool isReady(){ return status == STATUS_PLAYER_RSTART; }
		inline bool isActive(){ return status == STATUS_PLAYER_ACTIVE; }
		inline bool isGameOver(){ return status == STATUS_PLAYER_OVER; }
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


	public:
		int id;         //用户ID
		char name[64];
		short level;    //等级
		short seat_id;  //座位号
		short tid;		//当前属于哪个桌子ID
		short tab_index;//当前在桌子的位置
		int score;    //积分
		int64_t money;       //金币数
		int64_t carrycoin;	//这盘牌携带金币数
		int nwin;       //赢牌数
		int nlose;     //输牌数
		int nrunaway;	//逃跑的次数
		int ntie;
		char json[1024];        //json字符串
		short status;      //状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
		short source; //用户来源
		int clevel;       //当前金币场底注 0-无底注
		BYTE card_array[5];     //牌数
		bool hascard;		//是否有手牌
		short hallid;//当前这个用户在那个大厅ID
		short finalcardvalue; //最终手牌类型
		bool thisroundhasbet; //当前这轮是否已经下注
		bool hasallin;		//是否已经allin
		short optype;
		bool isorder;	//是否已经有序了
		bool isdropline; //是否已经掉线
		int voucher;	//兑换卷
		int deducte_tax; //抽掉的税
		char m_byIP[32];   //IP

		//统计字段
		short cid;
		short sid;
		short pid;
		//任务系统的版本控制
		bool istask;
		//超时下注的次数，只是第一轮就超时
		short timeoutCount;

		CoinCfg coincfg;
	//任务信息
	public:
		Task* task1;
		short ningot;
		//局数任务
		int boardTask;

		///////////////////////////////局数任务/////////////////////////////////////////////////
		bool isroundtask;
		int m_nRoundComFlag;						//局数任务完成的标志
		int m_nRoundNum;							//局数的数量用于判断是否完成局数任务
		BYTE m_bFinalComFlag;						//游戏结束完成局数任务的标志 0 没有完成 1 完成 2所有都完成
		int m_nComGetCoin;							//完成获得的金币数
		int m_nComGetRoll;							//完成获得的乐劵数
		short m_nMagicNum;
		int m_nMagicCoinCount;
	//准备信息，处理第一次进入要倒计时准备的问题
	public:
		short startnum;
		bool isbacklist;
	//下注信息 四轮的下注信息其中 betCoinList[0] 是此用户的总下注
	public:
		int64_t betCoinList[5];
	//最终获得的金币数，可为正也可为负
		int64_t finalgetCoin;
		int64_t nextlimitcoin;
	private:
		time_t active_time;//客户端最近活跃时间
		time_t enter_time;//进入游戏的时间
		time_t betcoin_time;//开始计算用户下注超时的时间
		time_t replay_time;//游戏等待时间，新一盘游戏开始时计时，用于机器人判断当前等待时间

};
#endif
