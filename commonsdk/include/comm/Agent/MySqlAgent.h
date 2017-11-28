

#pragma once

#include "BaseAgent.h"
#include "Typedef.h"

class MySqlAgent : public BaseAgent
{
public:
	MySqlAgent();
	virtual ~MySqlAgent();
	virtual bool init();

protected:
	virtual int responsePacket();
};