#include "threadres.h"

char* ThreadResource::getBuf(int len)
{
    pid_t pid = gettid();
    BUFMAP::iterator it=m_bufmap.find(pid);
    if(it == m_bufmap.end()) return allocBuf(len);

    return it->second.buf;
}

int ThreadResource::getBufLen()
{
    pid_t pid = gettid();
    BUFMAP::iterator it=m_bufmap.find(pid);
    if(it == m_bufmap.end()) return 0;
    
	return it->second.buflen;
}

char* ThreadResource::allocBuf(int len)
{
    pid_t pid = gettid();
    BUFMAP::iterator it=m_bufmap.find(pid);
    if(it == m_bufmap.end())
	{
        THREAD_STORAGE_BUF storage_buf;
        storage_buf.buflen = len;
        storage_buf.buf = (char*)malloc(len);
        m_bufmap.insert(BUFMAP::value_type(pid,storage_buf));
        return storage_buf.buf;
    }
    
	return it->second.buf;
}

void ThreadResource::releaseAllBuf()
{
    for(BUFMAP::iterator it=m_bufmap.begin(); it!=m_bufmap.end(); it++)
	{
        if(it->second.buf &&  it->second.buflen > 0)
		{
            free(it->second.buf);
        }
    }
}

void ThreadResource::releaseDBConn()
{
    for(DBMAP::iterator it=m_dbmap.begin(); it!=m_dbmap.end(); it++)
	{
		//删除前，需要先关闭所有库连接
		// it->second->CloseAllDbConn();
        delete it->second;
    }
}

void ThreadResource::releaseRedisConn()
{
	for(REDISMAP::iterator it=m_redismap.begin(); it!=m_redismap.end(); it++)
	{
		//删除前，需要先关闭所有连接
		// it->second->CloseAllConn();
		delete it->second;
	}
}

void ThreadResource::releaseEvtCliConn()
{
	for(EvtCliMAP::iterator it=m_evtCliMgrMap.begin(); it!=m_evtCliMgrMap.end(); it++)
	{
		//删除前，需要先关闭所有连接
		// it->second->CloseAllConn();
		delete it->second;
	}
}

MySqlDBAccessMgr* ThreadResource::getDBConnMgr()
{
    pid_t pid = gettid();
    DBMAP::iterator it=m_dbmap.find(pid);
    if (it == m_dbmap.end()) 
    {
        _LOG_INFO_("Init db access mgr for thread :%d", pid);
        MySqlDBAccessMgr *dbmgr =  new MySqlDBAccessMgr();
        dbmgr->Init();
        m_dbmap.insert(DBMAP::value_type(pid, dbmgr));
        return dbmgr;
    }

	return it->second;
}


RedisAccessMgr* ThreadResource::getRedisConnMgr()
{
	pid_t pid = gettid();
	REDISMAP::iterator it = m_redismap.find(pid);
	if (it == m_redismap.end()) 
	{
    	RedisAccessMgr *redismgr =  new RedisAccessMgr();
    	redismgr->Init();
    	m_redismap.insert(REDISMAP::value_type(pid, redismgr));
    	return redismgr;
    }
    
	return it->second;
}


CEvtClientMgr* ThreadResource::getCliConnMgr()
{
	pid_t pid = gettid();
	EvtCliMAP::iterator it = m_evtCliMgrMap.find(pid);
	if (it != m_evtCliMgrMap.end()) return it->second;

	CEvtClientMgr *evtClientMgr =  new CEvtClientMgr();
	evtClientMgr->Init();
	m_evtCliMgrMap.insert(EvtCliMAP::value_type(pid, evtClientMgr));
	return evtClientMgr;
}

ThreadResource::~ThreadResource()
{
    releaseAllBuf();
    releaseDBConn();
	releaseRedisConn();
	releaseEvtCliConn();
}
