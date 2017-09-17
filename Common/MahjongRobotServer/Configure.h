
#ifndef OPTIONS_H
#define OPTIONS_H

#include <time.h>
#include <string>
#include "Typedef.h"
#include "Game/GameConfig.h"

class Configure : public GameConfig
{
public:
	Singleton(Configure);

public:
	bool LoadGameConfig();

public:
	uint16_t port;
	uint16_t id;
	std::string name;
	std::string head_url;
	//扫描监控时间
	uint16_t monitor_time;
	//默认下注超时时间，可以再其上在加
	uint16_t base_bet_time;
	//连续和用户玩的牌局数
	uint16_t play_count;
	//用户等待时间就开启机器人
	uint16_t enter_time;
	// robot level
	uint16_t level;
	//机器人明牌打等级
	uint16_t konw_contrl_level;
	//第几个机器人服务器，从command line中获取,用于区分机器人id,必须从1开始
	uint16_t robot_server_index;

	int base_money[4];
	int rand_money[4];
	uint16_t rand_vip[5];
	uint16_t rand_call[4];
	int base_uid;
	int rand_uid;

public:
	std::string hall_ip;
	uint16_t hall_port;
	std::string serverxml;
};

#endif
