

#pragma once


#include "BaseAgent.h"
#include "Typedef.h"

class TrumptAgent : public BaseAgent
{
public:
	TrumptAgent();
	virtual ~TrumptAgent();
	int TrumptToUser(short type, const char* msg, short pid);

	virtual bool init();
	virtual int OnConnected();

protected:
	virtual int responsePacket() /*= 0*/;
};