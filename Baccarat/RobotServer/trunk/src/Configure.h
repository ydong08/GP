
#ifndef OPTIONS_H
#define OPTIONS_H

#include <time.h>
#include <unistd.h>
#include <string>
#include "Typedef.h"
#include "Game/GameConfig.h"

class Configure : public GameConfig
{
public:

	static Configure* getInstance() {
		static Configure * options = NULL;
		if(options==NULL)
			options = new Configure();
		return options;
	}

	virtual bool LoadGameConfig();

//***********************************************************
public:
	UINT16 m_robotId;	//第几个机器人服务器

	//连接大厅的ip和端口
	char hall_ip[64];
	short hall_port;

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

	short bankerlimit;
	short playerlimit;
	
	int bankerbase;
	int bankerrand;
	int playerbase;
	int playerrand;

	short playerplaybase;
	short playerplayrand;
	short betplaybase;
	short betplayrand;
	short bankerplaybase;
	short bankerplayrand;
	short bankerhasnum;

	int bankerwincount;

	int baseuid;
	int randuid;

	//头像链接
	std::string   m_headLink;
	std::string   serverxml;

private:
	Configure();
};

#define _CFG_(key)  Configure::getInstance()->key  
#endif
