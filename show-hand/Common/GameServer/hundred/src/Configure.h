
#ifndef _Configure_H
#define _Configure_H 

#include <time.h>
#include "Typedef.h"
#include <string>
#include "GameConfig.h"


#ifdef SHOWHAND
#define room_config_name	 "ShowhandMany"
#define robot_win_many_name  "RobotWinManyShowwhand"
#endif

#ifdef TEXAS
#define room_config_name    "TexasMany"
#define robot_win_many_name "RobotWinManyTexas"
#endif

#define MAX_ALLOC_NUM  8
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
	virtual int GetMaxTableNumber() { return max_table; }
	virtual int GetMaxUserNumber() { return max_user; }

//***********************************************************
public:

	std::string alloc_host;
	short alloc_port;
	short numplayer;

    int monitor_time;//监控时间
    int keeplive_time;//存活时间
	short idletime;
	short opentime;
	short bettime;

	int max_table;
	int max_user;
	//下注的超时时间
	int betcointime;
	//大于两个人准备其中有人没有准备，然后倒计时把没准备的人踢出，并且游戏开始
	int tablestarttime;
	//超时踢出没准备用户
	short kicktime;
	short reset_time;

	time_t starttime; //服务器启动时间
	time_t lasttime;  //最近活动时间

	int robotwin;

	//机器人进入排行榜的配置
	short switchrobotrank;

	//赢取金币小喇叭发送
	int wincoin1;
	int wincoin2;
	int wincoin3;
	int wincoin4;

	short switchsendsize;

	int bankerlimit;
	short bankernum;

	short rentrate;

	//聊天开关
	short switchchat;
	//控制庄家全赢的倍数 
	short bankerwintype;
	//控制天地玄黄某一处赢庄家的倍数
	short bankerlosetype;
	//控制赢的区域要大于几倍
	short areawintype;
	int switchbetmoney;
	int randlose;
	//库存到了这个值之后就启动开奖模式
	int jackpotSwitch;
	//开奖的比率
	int randopenjack;
	//系统下注占比
	int robotbetrate;

	short sendinfotime;

	short specialnum0;
	short specialnum1;
	short specialnum2;
	short specialnum3;
	short specialnum4;
	short specialnum5;
	short specialnum6;
private:
	Configure();
};

#endif
