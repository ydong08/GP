#pragma once

#include <vector>
#include <event.h>
#include <string>
#include <map>
#include "EvtTimer.h"

//typedef void (*EvHttpCallBack)(struct evhttp_request *);
typedef void (*EvHttpCallBack)(struct evhttp_request *, void *);

typedef struct PathCallBackParam_
{
	EvHttpCallBack funcCb;
	void * arg;
}PathCallBackParam;

class CEvHttpSvr
{
public:
	CEvHttpSvr(void);
	~CEvHttpSvr(void);
	static CEvHttpSvr& getInstance();
	void StopAllThread() ;
	// 主Dispatch线程的定时器
	void AddMainTimer(const EvTimerParam& evtTimer);
	// 线程定时器，每个线程都会执行
	void AddThreadTimer(pthread_t threadId, const EvTimerParam& evtTimer);
	void set_cb(const char *path, EvHttpCallBack funcCallBack, void *arg);
    int start(const std::string& ip, int port, int nthreads, int backlog, int nTimeOutSec, EvHttpCallBack funcCallBack, void *arg = NULL);
private:
	void AddThreadTimer(pthread_t threadId, event_base* ev_base);
	void set_cb(struct evhttp * evHttp);
	int dostart(int nthreads, int backlog, int nTimeOutSec, EvHttpCallBack funcCallBack, void *arg = NULL);
	static void* Dispatch(void *arg);
	//static void GenericHandler(struct evhttp_request *req, void *arg);
	//void ProccessRequest(struct evhttp_request *req);
	int bindsocket(int backlog, int nTimeOutSec);
	std::map<pthread_t, struct event_base *> evtMpThreadBase;

    std::string m_strIp;
	int m_nPort;
	int m_nThreads;
	int m_nBacklog;
	int m_nTimeOutSec;
	struct event_base* m_evMainBase;

	std::vector<CEvtTimer*> m_evTimerListMain;
	typedef  std::vector<EvTimerParam> VecTimerParam;
	typedef std::map<pthread_t, VecTimerParam> MP_TIMER_PARAM;
	MP_TIMER_PARAM m_evTimerParamThread;
	std::vector<CEvtTimer*> m_evTimerListThread;


	std::vector<struct evhttp *> m_evHttpLst;
	std::map<std::string, PathCallBackParam> m_evMpCbFunc;
	//EvHttpCallBack m_CallBackFunc;
};

