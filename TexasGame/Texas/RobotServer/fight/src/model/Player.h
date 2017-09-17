#ifndef PLAYER_H
#define PLAYER_H
#include <json/json.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "Typedef.h"
#include "GameLogic.h"
#include "HallHandler.h"
#include "PlayerTimer.h"

#define STATUS_TABLE_CLOSE	-1 //桌子关闭
#define STATUS_TABLE_EMPTY	 0 //桌子空
#define STATUS_TABLE_READY	 1 //桌子正在准备

#define STATUS_TABLE_ACTIVE 2	//桌子正在玩牌
#define STATUS_TABLE_OVER	 3	//桌子游戏结束

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

int pase_json(Json::Value& value, const char* key, int def);

typedef struct _PlayerCard
{
	int id;
	BYTE Knowcard_array[2];			//知道所以牌数
	bool hascard;
	int64_t betCoinList[5];
	short optype;
	int64_t limitcoin;
	int64_t diffcoin;
	short status;   
	char name[64];
	int64_t money;					//金币数
	int64_t carrycoin;				//这盘牌携带金币数
	char json[1024];				//json字符串
	short source;					//用户来源
}PlayerCard;

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
		inline void addPlayCount() {++play_count;};
		inline short getPlayCount() {return play_count;};
	//行为函数
	public:

		int login();
		int logout();
		int check();
		int call();
		int rase();
		int allin();
		int stargame();
		int throwcard();
		int heartbeat();
		bool robotIsLargest();
		bool robotKonwIsLargest();
		void setRaseCoin();
		void setKnowRaseCoin();
		int setRandRaseCoin( BYTE nMin, BYTE nMul);
		short getPlayNum();

	//时间函数
	public:	
		void stopAllTimer();
		void startBetCoinTimer(int uid,int timeout);
		void stopBetCoinTimer();
		void startLeaveTimer(int timeout);
		void stopLeaveTimer();
		void startActiveLeaveTimer(int timeout);
		void stopActiveLeaveTimer();
		void startHeartTimer(int uid,int timeout);
		void stopHeartTimer();
		void startStartGameTimer(int timeout);
		void stopStartGameTimer();

	public:
		short GetPlayerCardKind(BYTE bPlayerCardList[], BYTE bBeginPos, BYTE bEndPos);
		GameLogic m_GameLogic;		//游戏逻辑
	public:
		int id;						//用户ID
		char name[64];
		short seat_id;				//座位号
		int tid;					//当前属于哪个桌子ID
		short tab_index;			//当前在桌子的位置
		int score;					//积分
		int64_t money;				//金币数
		int64_t carrycoin;			//这盘牌携带金币数
		int nwin;					//赢牌数
		int nlose;					//输牌数
		char json[1024];			//json字符串
		short status;				//状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
		short source;				//用户来源
		int clevel;					//当前金币场底注 0-无底注
		BYTE card_array[2];			//牌数
		BYTE Knowcard_array[5];     //知道所以牌数
		BYTE CenterCard[5];
		bool hascard;				//是否有手牌
		short currcardvalue;
		short finalcardvalue;		//最终手牌类型
		bool thisroundhasbet;		//当前这轮是否已经下注
		bool hasallin;				//是否已经allin
		short optype;
		bool isdropline;			//是否已经掉线
		bool hasrase;

		int tax;

		bool isKnow;

		PlayerCard player_array[GAME_PLAYER];
		short playNum;				//玩多少局离开
		bool bactive;				//是否是已经牌桌开始了进去的
	public:
		int64_t ante;				//底注
		short raterent;				//税收的比率
		short countPlayer;			//当前桌子用户数量
		short currRound;			//当前处于下注的第几轮
		int64_t currMaxCoin;		//当前此轮下注最大的金额
		int64_t PoolCoin;			//当前底池的总数
		int raseUid;				//加注的用户
	//下注信息 四轮的下注信息其中 betCoinList[0] 是此用户的总下注
	public:
		int64_t betCoinList[5];
	//最终获得的金币数，可为正也可为负
		int64_t finalgetCoin;
		int64_t limitcoin;
	//通过计算得到用户加注数目
	public:
		int64_t rasecoin;
		HallHandler * handler;
	private:
		PlayerTimer timer ;
		short play_count;			//玩的盘数

};
#endif
