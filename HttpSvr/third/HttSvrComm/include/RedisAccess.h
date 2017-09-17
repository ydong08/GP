#ifndef _RedisAccess_H_
#define _RedisAccess_H_

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hiredis.h"
#include "Logger.h"
//#include "GameInfo.h"
#include <stdint.h>
#include "json/json.h"
#include <vector>
#include <string>
#include <map>
#include <event.h>

class RedisAccess
{
public:
	RedisAccess(const std::string& strRedisIP,const int& nRedisPort);
	virtual ~RedisAccess();

    bool connect();
	
	//************************************
	// Method:    CommandV	能处理key和value带空格的情况，请规范写法，把key和value放到最右边的...的位置
	// FullName:  RedisAccess::CommandV
	// Access:    public 
	// Returns:   redisReply *	
	// Qualifier:
	// Parameter: const char * format
	// Parameter: ...
	//************************************
	redisReply * CommandV(const char *format, ...);
	redisReply * CommandArgv(std::vector<const char *> &vtAll);
    int appendCommand(const char * format, ...);
    redisReply * applyBatchCommand();

    bool GET(const char* szkey, std::string& value);
    bool HGET(const char* szkey, const char* field, std::string& value);
    bool HMGET(const char* szKey,const std::vector<std::string>& vsFields,std::map<std::string,std::string>& mpRet);
    bool HGetAll(const char* szKey,std::map<std::string,std::string>& mpRet);

    bool SADD(const char* szKey, const char* szValue);
    bool HSET(const char* szKey, const char* field, const char* value);
    bool HMSET(const char* szKey, std::map<std::string,std::string>& mapKV);
    bool HMSET(const char* szKey, const Json::Value& mapKV);
	bool SET(const char* szkey, int value, int expire=0);
    bool SET(const char* szkey, const char* value, int expire=0);
    bool EXPIRE(const char* szkey, int expire);
	bool DEL(const char* szkey);
	int64_t INCRBY(const char* szKey, const int64_t nValue, int expire=0);
	std::string SPOP(const char* szkey);

    bool ZREVRANGE(const char* szKey, int start, int end, std::vector<std::string>& vec);
    
    struct DataScore
    {
        std::string key;
        int score;
    };
    bool ZREVRANGEWITHSCORE(const char* szKey, int start, int end, std::vector<DataScore>& vec);
    
    bool ZREVRANK(const char* szKey, int uid, int& rank);
    bool ZSCORE(const char* szKey, int uid, int& score);
    
    bool ping();

	std::string GetIp() {return m_strRedisIP;}
	int GetPort() {return m_nRedisPort;}
private:
	redisContext* initRedisContext(const char* ip, int port);
	std::string				m_strRedisIP;
	int						m_nRedisPort;
	redisContext			*m_pRedis;
};


#endif


