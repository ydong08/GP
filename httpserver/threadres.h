#ifndef __THREAD_RES_H__
#define __THREAD_RES_H__

#include <map>
#include <string>
#include <sys/syscall.h> 
#include <sys/types.h>
#if defined(linux) || defined(__linux) || defined(__linux__)
#include <unistd.h>
#include <sys/syscall.h>
#elif defined(__APPLE__)
#include <getopt.h>
#else
#error "Unknown platform. The supported target platforms are Linux and OSX."
#endif
//#include "MySqlDBAccess.h"
//#include "RedisAccess.h"
#include "EvtClientMgr.h"
#include "RedisAccessMgr.h"
#include "MySqlDBAccessMgr.h"

using namespace std;

typedef struct{
    unsigned int  buflen;
    char*         buf; 
}THREAD_STORAGE_BUF;

typedef struct{
    string host;
    string user;
    string passwd;
    string dbname;
    unsigned short port;
}MYSQL_PARAM;

class ThreadResource
{
typedef map<pid_t, THREAD_STORAGE_BUF, less<pid_t> > BUFMAP;
//typedef map<pid_t, MySqlDBAccess*, less<pid_t> > DBMAP;
typedef map<pid_t, MySqlDBAccessMgr*, less<pid_t> > DBMAP;
typedef map<pid_t, RedisAccessMgr*, less<pid_t> > REDISMAP;
typedef map<pid_t, CEvtClientMgr*, less<pid_t> > EvtCliMAP;

public:
    static const unsigned int _s_max_buflen = 1024*1024;
public:
    ~ThreadResource();
    char* getBuf(int len = _s_max_buflen);
    int   getBufLen();

    MySqlDBAccessMgr* getDBConnMgr();
	RedisAccessMgr* getRedisConnMgr();
	CEvtClientMgr* getCliConnMgr();
	
	static ThreadResource& getInstance()
	{
		static ThreadResource inst;
		return inst;
	}
	
private:
    ThreadResource()
    {
        m_bufmap.clear();
        m_dbmap.clear();
        m_redismap.clear();
		m_evtCliMgrMap.clear();
    }

    char* allocBuf(int len = _s_max_buflen);

    pid_t gettid()
    {
        return syscall(SYS_gettid);
    }

	void releaseAllBuf();
	void releaseDBConn();
	void releaseRedisConn();
	void releaseEvtCliConn();

private:
    BUFMAP m_bufmap;
    DBMAP  m_dbmap;
	REDISMAP m_redismap;
	EvtCliMAP m_evtCliMgrMap;
};

#endif
