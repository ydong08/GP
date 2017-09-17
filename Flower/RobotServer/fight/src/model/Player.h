#ifndef PLAYER_H
#define PLAYER_H
#include <json/json.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "GameLogic.h"
#include "HallHandler.h"
#include "PlayerTimer.h"
using namespace std;

const int GAME_PLAYER = 5;

#define STATUS_TABLE_CLOSE		-1	//桌子关闭
#define STATUS_TABLE_EMPTY		0	//桌子空
#define STATUS_TABLE_READY		1	//桌子正在准备
#define STATUS_TABLE_ACTIVE		2	//桌子正在玩牌
#define STATUS_TABLE_OVER		3	//桌子游戏结束

#define CARD_MAX 5

#define STATUS_PLAYER_LOGOUT 0
#define STATUS_PLAYER_LOGIN  1  //登陆，未入座
#define STATUS_PLAYER_COMING 2  //入座
#define STATUS_PLAYER_RSTART 3  //入座，准备开始
#define STATUS_PLAYER_ACTIVE 4  //正在打牌
#define STATUS_PLAYER_OVER   5  //游戏结束

#define PLAYER_CALL_BANKER	1
#define PLAYER_NOT_CALL		2

#define OP_CALL		1	//跟注
#define OP_RASE		2	//加注
#define OP_ALLIN	4	//全下
#define OP_CHECK	8	//看牌
#define OP_THROW	16	//放弃
#define OP_COMPARE	32	//比牌
#define OP_CALLING	64	//控制跟到底


int pase_json(Json::Value& value, const char* key, int def);

typedef struct _PlayerCard
{
	int id;
	BYTE card_array[3];		//牌数
	BYTE Knowcard_array[3];	//知道所以牌数
	short status;   
	char name[64];
	int64_t money;			//金币数
	bool isbanker;
	short hascard;
	short mul;
	char json[512];			//json字符串
	short source;			//用户来源
	short forbidround;
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
		int heartbeat();
		int gamestart();
		bool robotKonwIsLargest();
		int call();
		int rase();
		int throwcard();
		int allin();
		int comparecard();
		int check();
		bool isCanGameOver();
		bool isRaseMax();
		bool hasSomeCheck();
		void setRaseCoin();

	//时间函数
	public:	
		void stopAllTimer();
		void startLeaveTimer(int timeout);
		void stopLeaveTimer();
		void startHeartTimer(int uid,int timeout);
		void stopHeartTimer();

		void startGameStartTimer(int uid,int timeout);
		void stopGameStartTimer();
		void startBetCoinTimer(int uid,int timeout);
		void stopBetCoinTimer();
		void startCheckTimer(int uid,int timeout);
		void stopCheckTimer();

	public:
		int  m_nAwayCount;
		bool m_bBetCoinTimer;
		int  m_betCoinTime;
		bool m_bCheckTimer;
		int  m_checkTime;
		std::vector<int> m_vAwayUid;


	public:
		GameLogic m_GameLogic; //游戏逻辑
	public:
		int id;         //用户ID
		char name[64];
		short seat_id;  //座位号
		int tid;		//当前属于哪个桌子ID
		short tab_index;//当前在桌子的位置
		short status;
		short tstatus;
		short source;
		int score;    //积分
		int64_t money;       //金币数
		short clevel;
		BYTE card_array[3];     //牌数
		BYTE Knowcard_array[3];     //知道所以牌数
		char json[1024];        //json字符串
		short hascard; 
		bool isbanker;
		int bankeruid;
		int compareUid;
		BYTE CardType;
		int m_nMulArray[4];
		BYTE maxValue;

		short optype;

		short iscall;
		short mulindex;
		int64_t sumbetcoin;

		bool isKnow;

		PlayerCard player_array[GAME_PLAYER];
		short playCount;
		short playNum; //玩多少局离开
		bool bactive;//是否是已经牌桌开始了进去的
		//是否已经发送了看牌
		bool bcheck;

		int ante;
		int64_t currmax;
		int64_t rasecoin;

		int rasecoinarray[5];

		short currRound;
	
		short countnum;
		HallHandler * handler;
	private:
		PlayerTimer timer ;
		short play_count; //玩的盘数

};
#endif
