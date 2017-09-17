

#pragma once

#include <ctime>
#include <cstdint>
#include <string>

#include "Protocol.h"

enum PlayerStatus
{
	STATUS_INVALID = -1,
	STATUS_PLAYER_LOGOUT,
	STATUS_PLAYER_LOGIN,
	STATUS_PLAYER_COMING,
	STATUS_PLAYER_RSTART,
	STATUS_PLAYER_ACTIVE,
	STATUS_PLAYER_OVER,
	STATUS_COUNT
};

class BasePlayer
{
public:
	virtual ~BasePlayer() {}

public:
	bool isLogout() const { return status == STATUS_PLAYER_LOGOUT; }
	bool isLogin() const { return status == STATUS_PLAYER_LOGIN; }
	bool isComming() const { return status == STATUS_PLAYER_COMING; }
	bool isReady() const { return status == STATUS_PLAYER_RSTART; }
	bool isActive() const { return status == STATUS_PLAYER_ACTIVE; }
	bool isGameOver() const { return status == STATUS_PLAYER_OVER; }

	void setActiveTime(time_t t) { active_time = t; }
	time_t getActiveTime() const { return active_time; }
	void   setEnterTime(time_t t) { enter_time = t; }
	time_t getEnterTime() const { return enter_time; }
	void   setRePlayeTime(int t) { replay_time = t; }
	int    getRePlayeTime() const { return replay_time; }
	void   setTimeOutTime(int t) { timeout_time = t; }
	int    getTimeOutTime() const { return timeout_time; }

	bool isRobot() const { return source == E_MSG_SOURCE_ROBOT; }

	virtual void login();
	virtual void enter();
	virtual void leave();
	virtual bool checkMoney();

protected:
	short status;
	short source;

	int id;
	int hallid;
	short tableid;

	int64_t money;
	int64_t win_money;	//本局赢取的金币
	int64_t ded_tax;	//本局上交的税

	std::string name;
	std::string json;
	std::string headlink;

	time_t active_time;							//客户端最近活跃时间
	time_t enter_time;							//进入游戏的时间
	time_t replay_time;							//游戏等待时间，新一盘游戏开始时计时，用于机器人判断当前等待时间
	time_t timeout_time;						//游戏给他设置超时时间的时刻
};