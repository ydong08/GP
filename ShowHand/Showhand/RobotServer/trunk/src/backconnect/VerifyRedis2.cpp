#include "VerifyRedis2.h"
#include "Logger.h"
#include "Options.h"
#include "hiredis.h"
#include "Util.h"
#include <vector>
using namespace std;

static VerifyRedis2* instance = NULL;

VerifyRedis2* VerifyRedis2::getInstance()
{
	if(instance==NULL)
	{
		instance = new VerifyRedis2();
	}
	return instance;
}

VerifyRedis2::VerifyRedis2()
{
    heartBeatTimer = new Redis2Timer();
    heartBeatTimer->init(this);
    heartBeatTimer->StartTimer(60*1000);
    m_redis = NULL;
    reply = NULL;
	replyres = NULL;
}

VerifyRedis2::~VerifyRedis2()
{
    delete heartBeatTimer;
    if(m_redis)
    {
        redisFree(m_redis);
        m_redis = NULL;
    }
}
int VerifyRedis2::InitRedis()
{
	if(m_redis)
        redisFree(m_redis);

    m_redis = Util::initRedisContext(Options::instance()->verifyredis_ip2, Options::instance()->verifyredis_port2);
    if (NULL == m_redis)
		return -1;
    
	_LOG_INFO_("Initredis-connect OK...\n");
	return 0;
}


int VerifyRedis2::getUserNameSex(int &uid, char *nick, short &sex)
{
	if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return -1;
    }

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "randomkey"));
    if(reply == NULL)
    {
		LOGGER(E_LOG_WARNING)<< "Redis Server is Closed, Can't Connet RedisServer";
        if(InitRedis() < 0)
            return -1;
		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "randomkey"));
        if(reply == NULL)
		{
            return -1;
		}
    }
	if(reply->type != REDIS_REPLY_STRING)
	{
		LOGGER(E_LOG_WARNING)<< "VerifyRedis reply is type["<<reply->type<<"] uid:"<<uid;
		freeReplyObject(reply);
		return -1;
	}

	if(strlen(reply->str) < 5)
	{
		freeReplyObject(reply);
		return -1;
	}

	if(reply->str[0] == 'U' && reply->str[1] == 'C' && reply->str[2] == 'N' && reply->str[3] == 'F')
	{
		char idbuff[10]={0};
		memcpy(idbuff, reply->str + 4, strlen(reply->str) - 4);
		uid = atoi(idbuff);
		freeReplyObject(reply);
		redisReply *idreply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "hmget UCNF%d mnick sex", uid)); 
		if(idreply == NULL)
		{
			LOGGER(E_LOG_WARNING)<< "Redis Server is Closed, Can't Connet RedisServer";
			if(InitRedis() < 0)
				return -1;
			idreply = reinterpret_cast<redisReply*>(redisCommand(m_redis,"hmget UCNF%d mnick sex", uid)); 
			if(idreply == NULL)
				return -1;
		}
		int ret = 0;
		if (idreply->type == REDIS_REPLY_ARRAY) {
			for (unsigned int j = 0; j < idreply->elements; j++) {
				if(idreply->element[j]->type==REDIS_REPLY_NIL)
					continue;
				switch (j)
				{
					case 0: strcpy(nick, idreply->element[j]->str);break;
					case 1: sex = atoi(idreply->element[j]->str);break;
				}				
			}
			if(idreply->elements==0){
				LOGGER(E_LOG_WARNING)<< "["<<__FILE__<<":"<<__LINE__<<"] Can't get configure UCNF"<<uid<<",user maybe not in redis";
				ret = -1;
			}
		}else if(idreply->type == REDIS_REPLY_ERROR){
			LOGGER(E_LOG_WARNING)<< "["<<__FILE__<<":"<<__LINE__<<"] Redis_Error["<<idreply->type<<"]["<<idreply->str<<"]";
			ret = -1;
		}
		freeReplyObject(idreply);
		return ret;
	}
	else
	{
		freeReplyObject(reply);
		return -1;
	}
}

int VerifyRedis2::pingSrv()
{
    if(m_redis == NULL)
        return -1;
    reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ping"));
    if(reply == NULL)
        return -1;
	freeReplyObject(reply);
    return 0;
}

//=============================Redis2Timer===============================
int Redis2Timer::ProcessOnTimerOut()
{
	//_LOG_DEBUG_("\n");
	//_LOG_DEBUG_( "==============Redis2Timer===========\n" );
	LOGGER(E_LOG_DEBUG)<< "==============Redis2Timer===========";
	if(redissrv)
	{
		if(redissrv->pingSrv()==0)
		{
			LOGGER(E_LOG_DEBUG)<< "Ping Redis Server OK";
		}
		else
        {
			LOGGER(E_LOG_WARNING)<< "Ping Redis Server ERROR, Need ReConnect ";
            redissrv->InitRedis();
        }
	}
	else
	{
		LOGGER(E_LOG_DEBUG)<< "Redis Server DisConnect, Need ReConnect ...";
		redissrv->InitRedis();
	}
	LOGGER(E_LOG_DEBUG)<< "=======================================";
	StartTimer(60*1000);
	return 0;
}

