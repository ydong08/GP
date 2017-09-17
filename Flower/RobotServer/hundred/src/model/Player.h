#ifndef PLAYER_H
#define PLAYER_H
#include <string.h>
#include <time.h>
#include <stdint.h>
//#include "wtypedef.h"
#include "PlayerTimer.h"

using namespace std;


#define STATUS_TABLE_CLOSE	-1 //桌子关闭
#define STATUS_TABLE_IDLE	 1 //桌子空闲
#define STATUS_TABLE_BET	 2 //桌子正在下注
#define STATUS_TABLE_OPEN    3	//桌子正在开牌

class HallHandler;

class Player
{
	public:
		Player(){};
		virtual ~Player(){};
		void init();

	//内置函数
	public:
	//行为函数
	public:
		int login();
		int logout();
		int heartbeat();
		int betCoin();

	public:
		void startHeartTimer(int ntime);
		void startBetTimer(int timeout);
		void stopBetTimer();

	public:
		int id;         //用户id
		char name[64];
		short seat_id;  //座位号
		int tid;		//当前属于哪个桌子id
		short tab_index;//当前在桌子的位置
		int64_t money;       //金币数
		int nwin;       //赢牌数
		int nlose;     //输牌数
		char json[1024];        //json字符串
		short status;      //状态 0 未登录 1 在玩牌  
		short source; //用户来源
		int clevel;       //当前金币场底注 0-无底注
		bool isdropline; //是否已经掉线
		int level;
		short bettype;
		int64_t betcoin;

		int bankeruid;
		int64_t bankerwinCount;

		short betNum;//下注的次数
		short playNum;
		short bankerhasNum;

		short willPlayCount;

	//下注信息 四轮的下注信息其中 betCoinList[0] 是此用户的总下注
	public:
		HallHandler * handler;
	private:
		PlayerTimer timer;

};
#endif
