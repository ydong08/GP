

#pragma once

#include "BaseAgent.h"
#include "Typedef.h"

typedef void(*pfnGetRoundInfo)(InputPacket* packet);

class RoundAgent : public BaseAgent
{
public:
	RoundAgent();
	virtual ~RoundAgent();
	virtual bool init();

    int GetRoundInfo(int uid, int tid);
	void SetGetRoundInfoCallback(pfnGetRoundInfo cb) { m_cbGetRoundInfo = cb; }

protected:
	virtual int responsePacket();

	void onGetRoundInfo();

protected:
    pfnGetRoundInfo m_cbGetRoundInfo;
};