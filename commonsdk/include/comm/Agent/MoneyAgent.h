

#pragma once


#include "BaseAgent.h"
#include "Typedef.h"
#include <stdint.h>


class MoneyAgent : public BaseAgent
{
public:
	MoneyAgent();
	virtual ~MoneyAgent();
	virtual bool init();
	virtual int OnConnected();

	virtual int updateMoney(int uid, int64_t opmoney);

protected:
	virtual int responsePacket();
};