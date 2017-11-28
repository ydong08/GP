
#ifndef _Configure_H
#define _Configure_H 

#include "GameConfig.h"

class Configure : public GameConfig
{
public:

	static Configure* getInstance() {
		static Configure * configure = NULL;
		if(configure==NULL)
			configure = new Configure();
		return configure;
	}

    virtual bool LoadGameConfig();
//***********************************************************
public:
    short server_id; //服务器ID
    short level;//服务器等级
    int server_priority;    // 优先权
    
	char alloc_ip[64];
	short alloc_port;
	short numplayer;

	//第一轮最大倍数
	short maxmulone;
	//第二轮最大倍数
	short maxmultwo;

    int monitor_time;//监控时间
    int keeplive_time;//存活时间

	int max_table;
	int max_user;
	//下注的超时时间
	int betcointime;
	//大于两个人准备其中有人没有准备，然后倒计时把没准备的人踢出，并且游戏开始
	int tablestarttime;
	//超时踢出没准备用户
	short kicktime;
	short privatetime;

	time_t starttime; //服务器启动时间
	time_t lasttime;  //最近活动时间
	//下低注就超时几次之后就把他踢出
	short timeoutCount;

	short fraction;

	//简单任务的发放局数条件
	short esayTaskCount;
	short esayRandNum;
	short esayTaskRand;
	//当天前n局
	short curBeforeCount;
	//当天前N局游戏获取简单任务的概率
	short esayTaskProbability;
	//获得乐劵通知的个数条件
	short getIngotNoti1;
	short getIngotNoti2;
	short getIngotNoti3;

	short robotTabNum1;
	short robotTabNum2;
	short robotTabNum6;

	int rewardcoin;
	int rewardroll;
	short rewardRate;

	//赢取金币小喇叭发送
	int wincoin1;
	int wincoin2;
	int wincoin3;
	int wincoin4;

private:
	Configure();
};

#endif
