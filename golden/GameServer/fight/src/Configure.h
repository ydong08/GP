
#ifndef _Configure_H
#define _Configure_H 

#include <time.h>
#include "Util.h"
#include "GameConfig.h"

#define MAX_ALLOC_NUM  8

class Configure : public GameConfig
{
public:
	static Configure* getInstance() {
		static Configure * configure = NULL;
		if (configure == NULL)
			configure = new Configure();
		return configure;
	}

	virtual bool LoadGameConfig();
	virtual int GetMaxTableNumber() { return max_table; }
	virtual int GetMaxUserNumber() { return max_user; }

//***********************************************************
public:
	char alloc_ip[64];
	short alloc_port;
	short numplayer;

	//控制进入桌子
	short contrllevel;

	//第一轮最大倍数
	short maxmulone;
	//第二轮最大倍数
	short maxmultwo;

    int monitor_time;//监控时间
    int keeplive_time;//存活时间

	//总轮数
	short max_round;
	//比牌轮数
	//short compare_round;
	//全下轮数
	short allin_round;
	//short check_round;
	//下注的超时时间
	int betcointime;
	//大于两个人准备,另有人没有准备，启动倒计时把没准备的人踢出且游戏开始
	int tablestarttime;
	//超时踢出没准备用户
	short kicktime;

	time_t starttime; //服务器启动时间
	time_t lasttime;  //最近活动时间
	//下低注就超时几次之后就把他踢出
	short timeoutCount;

	short fraction;

	//简单任务的发放局数条件
	short esayTaskCount;
	short esayRandNum;
	short esayTaskRand;

	//获得乐劵通知的个数条件
	short getIngotNoti1;
	short getIngotNoti2;
	short getIngotNoti3;

	short robotTabNum1;
	short robotTabNum2;

	int rewardcoin;
	int rewardroll;
	short rewardRate;
	
	//赢取金币小喇叭发送
	int wincoin1;
	int wincoin2;
	int wincoin3;
	int wincoin4;

	short mulcount;
	short mulnum1;
	int mulcoin1;
	short mulnum2;
	int mulcoin2;

	short forbidcount;
	short forbidnum1;
	int forbidcoin1;
	short forbidnum2;
	int forbidcoin2;

	short changecount;
	int changecoin1;
	int changecoin2;
	int changecoin3;

	short betmaxallinflag;

private:
	Configure();

private:
	int max_table;
	int max_user;
};

#endif
