
#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <time.h>
#include <unistd.h>
#include "Typedef.h"
#include <string>
#include "Game/GameConfig.h"
 
class Configure : public GameConfig
{
public:

	static Configure* instance() {
		static Configure * options = NULL;
		if(options==NULL)
			options = new Configure();
		return options;
	}
    virtual bool LoadGameConfig();


//***********************************************************
public:
	time_t lasttime;
	//扫描监控时间
	short monitortime;
	//默认下注超时时间，可以再其上在加
	short basebettime;
	//连续和用户玩的牌局数
	short baseplayCount;
	//用户等待时间就开启机器人
	short entertime;

	short level;
	//机器人明牌打等级
	short konwcontrlLevel;

	int swtichWin1;
	int swtichWin2;
	int swtichWin6;

	int swtichOnKnow1;
	int swtichOnKnow2;
	int swtichOnKnow6;

	int baseMoney1;
	int baseMoney2;
	int baseMoney3;
	int baseMoney6;
	int randMoney1;
	int randMoney2;
	int randMoney6;

	int baseuid;
	int randuid;

	//机器人知道底牌的概率
	short randknow;
	short randhead;

	UINT16 m_robotId;	//第几个机器人服务器

    char hall_ip[64];
    unsigned short hall_port;

    std::string serverxml;

private:
	Configure();
};

#define _CFG_(key)  Configure::instance()->key  
#endif
