

#pragma once

#include "BaseAgent.h"
#include "Typedef.h"

class UserAgent : public BaseAgent
{
public:
	UserAgent();
	virtual ~UserAgent();
	virtual void userEnterGame(int uid, int tid);
	virtual void userLeaveGame(int uid);

	virtual bool init();
	virtual bool init(const std::string& ip, uint16_t port);

	static bool checkUserCountUpLimit(int currUser, int maxUser, int upRate, bool& openUpLimit);
	static bool checkUserCountDownLimit(int currUser, int maxUser, int upRate, int downRate, bool& openUpLimit);

protected:
	virtual void onSyncStatusResp();

	virtual int responsePacket() /*= 0*/;
};