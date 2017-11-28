#include "RobotRedis.h"
#include "Logger.h"
#include "Options.h"
#include "hiredis.h"
#include "Util.h"
#include <vector>
using namespace std;

static RobotRedis* instance = NULL;

RobotRedis* RobotRedis::getInstance()
{
	if(instance==NULL)
	{
		instance = new RobotRedis();
	}
	return instance;
}

RobotRedis::RobotRedis()
{
    heartBeatTimer = new RedisPingTimer();
    heartBeatTimer->init(this);
    heartBeatTimer->StartTimer(60*1000);
    m_redis = NULL;
    reply = NULL;
}

RobotRedis::~RobotRedis()
{
    delete heartBeatTimer;
    if(m_redis)
    {
        redisFree(m_redis);
        m_redis = NULL;
    }
}
int RobotRedis::InitRedis()
{
	if(m_redis)
        redisFree(m_redis);
    m_redis = Util::initRedisContext(Options::instance()->redis_ip, Options::instance()->redis_port);
    if (NULL == m_redis)
		return -1;
	
	_LOG_INFO_("Initredis-connect OK...\n");
    return 0;
}

std::string RobotRedis::readHeadUrl()
{
	std::string headlink;
	if (m_redis == NULL)
	{
		if (InitRedis() < 0)
			return headlink;
	}
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "get robotheadurl"));
	if (reply == NULL)
	{
		LOGGER(E_LOG_ERROR) << "Redis Server is Closed, Can't connect RedisServer";
		if (InitRedis() < 0)
			return headlink;
		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "get robotheadurl"));
		if (reply == NULL)
			return headlink;
	}
	if (reply->type == REDIS_REPLY_STRING)
		headlink = reply->str;
	freeReplyObject(reply);
	return headlink;
}

int64_t RobotRedis::getRobotWinCount(short level)
{
    if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return -1;
    }
    reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "hget robotwininfo level%d", level));
    //printf("reply is [%d]\n", reply->type);
    if(reply == NULL)
    {
        //_LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
		LOGGER(E_LOG_INFO) << "Redis Server is Closed, Can't Connet RedisServer";
        if(InitRedis() < 0)
            return -1;
        reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "hget robotwininfo level%d", level));
        if(reply == NULL)
            return -1;
    }
    //如果返回的type是空则说明查找失败
    if(reply->type == REDIS_REPLY_NIL)
    {
		freeReplyObject(reply);
		return -1;
	}
    int64_t money = 0;
	if(reply->type == REDIS_REPLY_STRING)
		money = atol(reply->str);
    freeReplyObject(reply);
    return money;
}

int RobotRedis::AddRobotWin(short level, int64_t money)
{
    if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return -1;
    }
    reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HINCRBY robotwininfo level%d %ld", level, money));
    //printf("reply is [%d]\n", reply->type);
    if(reply == NULL)
    {
		LOGGER(E_LOG_WARNING) << "Redis Server is Closed, Can't Connet RedisServer";
      //  _LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
        if(InitRedis() < 0)
            return -1;
        reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HINCRBY robotwininfo level%d %ld", level, money));
        if(reply == NULL)
            return -1;
    }
    //如果返回的type是空则说明查找失败
    if(reply->type == REDIS_REPLY_NIL)
    {
		freeReplyObject(reply);
		return -1;
	}
    freeReplyObject(reply);
    return 0;
}

int RobotRedis::pingSrv()
{
    if(m_redis == NULL)
        return -1;
    reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ping"));
    if(reply == NULL)
        return -1;
	freeReplyObject(reply);
    return 0;
}

//=============================RedisPingTimer===============================
int RedisPingTimer::ProcessOnTimerOut()
{
	//_LOG_DEBUG_("\n");
//	_LOG_DEBUG_( "==============RedisPingTimer===========\n" );
	LOGGER(E_LOG_DEBUG) << "==============RedisPingTimer===========";
	if(redissrv)
	{
		if(redissrv->pingSrv()==0)
			LOGGER(E_LOG_DEBUG) << "Ping Redis Server OK";

			//_LOG_DEBUG_( "Ping Redis Server OK\n" );
		else
        {
			LOGGER(E_LOG_WARNING) << "Ping Redis Server ERROR, Need ReConnect ";
		//	_LOG_ERROR_( "Ping Redis Server ERROR, Need ReConnect \n" );
            redissrv->InitRedis();
        }
	}
	else
	{
		//_LOG_DEBUG_( "Redis Server DisConnect, Need ReConnect ...\n" );
		LOGGER(E_LOG_DEBUG) << "Redis Server DisConnect, Need ReConnect ...";
		redissrv->InitRedis();
	}
	//_LOG_DEBUG_( "=======================================\n" );
	LOGGER(E_LOG_DEBUG) << "=======================================";
	StartTimer(60*1000);
	return 0;
}

