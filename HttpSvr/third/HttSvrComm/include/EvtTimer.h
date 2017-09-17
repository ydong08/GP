#pragma once
#ifndef EvtTimer_h__
#define EvtTimer_h__
#include <event.h>

typedef void (*evt_timer_cb)(void* context);

typedef struct EvTimerParam_
{
	struct timeval tvTimer;
	evt_timer_cb timerCb;
	void* context;

}EvTimerParam, *PEvTimerParam;

class CEvtTimer
{
public:
	CEvtTimer(struct timeval interval, evt_timer_cb evtTimerCb, void* context);
	CEvtTimer(const EvTimerParam& evTimerParma );
	~CEvtTimer(void);

	bool start(event_base* base);

	static void callback(int fd, short _event, void *argc);

private:
	struct event *m_evTimeout;
	EvTimerParam m_evTimerParam;
};


#endif // EvtTimer_h__

