#include "TaskRedis.h"
#include "Logger.h"
#include "Configure.h"
#include "hiredis.h"
#include "Player.h"
#include "Table.h"
#include "Util.h"
#include <vector>
using namespace std;

static TaskRedis* instance = NULL;

TaskRedis* TaskRedis::getInstance()
{
	if(instance==NULL)
	{
		instance = new TaskRedis();
	}
	return instance;
}

TaskRedis::TaskRedis()
{
    heartBeatTimer = new RedisTTimer();
    heartBeatTimer->init(this);
    heartBeatTimer->StartTimer(60*1000);
    m_redis = NULL;
    reply = NULL;
	replyres = NULL;
}

TaskRedis::~TaskRedis()
{
    delete heartBeatTimer;
    if(m_redis)
    {
        redisFree(m_redis);
        m_redis = NULL;
    }
}
int TaskRedis::InitRedis()
{
	if(m_redis)
        redisFree(m_redis);

    m_redis = Util::initRedisContext(Configure::getInstance()->taskredis_ip, Configure::getInstance()->taskredis_port);
    if (NULL == m_redis)
		return -1;
    
	_LOG_INFO_("Initredis-connect OK...\n");
    return 0;
}

int TaskRedis::getCompleteFlag(int uid)
{
	if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return -1;
    }

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET roundtaskcomhash %d", uid));
    if(reply == NULL)
    {
        _LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
        if(InitRedis() < 0)
            return -1;
		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET roundtaskcomhash %d", uid));
        if(reply == NULL)
            return -1;
    }

	int comflag = 0;
	if(reply->type == REDIS_REPLY_STRING)
	{
		comflag = atoi(reply->str);
	}
	freeReplyObject(reply);
	return comflag;
}

int TaskRedis::getPlayCount(int uid)
{
	if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return -1;
    }

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET roundtaskhash %d", uid));
    if(reply == NULL)
    {
        _LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
        if(InitRedis() < 0)
            return -1;
		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET roundtaskhash %d", uid));
        if(reply == NULL)
            return -1;
    }

	int playecount = 0;
	if(reply->type == REDIS_REPLY_STRING)
	{
		playecount = atoi(reply->str);
	}
	freeReplyObject(reply);
	return playecount;
}

//获取当天完成的局数
int TaskRedis::getPlayerCompleteCount( int uid )
{
	int retCount = 0;
	
	if( m_redis == NULL )
	{
		if( InitRedis() < 0 )
			return -1;
	}

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET sh_winstreak %d", uid));
	if( reply == NULL )
	{
		_LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
		if(InitRedis() < 0)
			return -1;

		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET sh_winstreak %d", uid));
		if( reply == NULL )
			return -1;
	}

	if( reply->type != REDIS_REPLY_STRING )
		return -1;

	retCount = strlen( reply->str );
	freeReplyObject(reply);
	
	return retCount;
}

int TaskRedis::setPlayerRound(Table* table, Player* player)
{ 
	if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return -1;
    }
	if(table == NULL || player == NULL)
		return -1;

	if(Configure::getInstance()->level == 4)
		return 0;
	int comflag = 0;
	int roundnum = 0;
	if(Configure::getInstance()->level == 1)
	{
		comflag = player->m_nRoundComFlag & 0x0F;
		roundnum = player->m_nRoundNum & 0x00FF;
	}
	if(Configure::getInstance()->level == 2)
	{
		comflag = (player->m_nRoundComFlag >> 4) & 0x0F;
		roundnum = (player->m_nRoundNum >> 8) & 0x00FF;
	}
	if(Configure::getInstance()->level == 3)
	{
		comflag = (player->m_nRoundComFlag >> 8) & 0x0F;
		roundnum = (player->m_nRoundNum >> 16) & 0x00FF;
	}
	++roundnum;
	if(table->m_nRoundNum3 != 0)
	{
		//都完成了
		if(comflag & 4)
		{
			player->m_bFinalComFlag = 2;
			return 0;
		}
		//完成中间的
		else if (comflag & 2)
		{
			if(roundnum == (table->m_nRoundNum3 + table->m_nRoundNum2 + table->m_nRoundNum1))
			{
				comflag = comflag | 4;
				player->m_bFinalComFlag = 1;
			}
		}
		//完成第一个局数
		else if (comflag & 1)
		{
			if(roundnum == (table->m_nRoundNum2 + table->m_nRoundNum1))
			{
				comflag = comflag | 2;
				player->m_bFinalComFlag = 1;
			}
		}
		//所有的都没有完成
		else
		{
			if(roundnum == table->m_nRoundNum1)
			{
				comflag = comflag | 1;
				player->m_bFinalComFlag = 1;
			}
		}
	}
	else if(table->m_nRoundNum2 != 0)
	{
		//完成中间的
		if (comflag & 2)
		{
			player->m_bFinalComFlag = 2;
			return 0;
		}
		//完成第一个局数
		else if (comflag & 1)
		{
			if(roundnum == (table->m_nRoundNum2 + table->m_nRoundNum1))
			{
				comflag = comflag | 2;
				player->m_bFinalComFlag = 1;
			}
		}
		//所有的都没有完成
		else
		{
			if(roundnum == table->m_nRoundNum1)
			{
				comflag = comflag | 1;
				player->m_bFinalComFlag = 1;
			}
		}
	}
	else if (table->m_nRoundNum1 != 0)
	{
		//完成第一个局数
		if (comflag & 1)
		{
			player->m_bFinalComFlag = 2;
			return 0;
		}
		//所有的都没有完成
		else
		{
			if(roundnum == table->m_nRoundNum1)
			{
				comflag = comflag | 1;
				player->m_bFinalComFlag = 1;
			}
		}
	}
	else
	{
		player->m_bFinalComFlag = 0;
		return 0;
	}

	if(Configure::getInstance()->level == 1)
	{
		player->m_nRoundComFlag |= comflag;
		player->m_nRoundNum = player->m_nRoundNum & 0xFFFFFF00 |roundnum;
	}
	if(Configure::getInstance()->level == 2)
	{
		player->m_nRoundComFlag |= comflag << 4;
		player->m_nRoundNum = player->m_nRoundNum & 0xFFFF00FF | roundnum << 8;
	}
	if(Configure::getInstance()->level == 3)
	{
		player->m_nRoundComFlag |= comflag << 8;
		player->m_nRoundNum = player->m_nRoundNum & 0xFF00FFFF | roundnum << 16;
	}

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HSET roundtaskhash %d %d", player->id, player->m_nRoundNum));
	replyres = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HSET roundtaskcomhash %d %d", player->id, player->m_nRoundComFlag));
    if(reply == NULL || replyres == NULL)
    {
        _LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
        if(InitRedis() < 0)
		{
			player->m_bFinalComFlag = 0;
			player->m_nComGetCoin = 0;
            return -1;
		}
        reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HSET roundtaskhash %d %d", player->id, player->m_nRoundNum));
		replyres = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HSET roundtaskcomhash %d %d", player->id, player->m_nRoundComFlag));
        if(reply == NULL || replyres == NULL)
		{
			player->m_bFinalComFlag = 0;
			player->m_nComGetCoin = 0;
            return -1;
		}
    }

	freeReplyObject(reply);
	freeReplyObject(replyres);
	return 0;
}


int TaskRedis::pingSrv()
{
    if(m_redis == NULL)
        return -1;
    reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ping"));
    if(reply == NULL)
        return -1;
	freeReplyObject(reply);
    return 0;
}

//=============================RedisTTimer===============================
int RedisTTimer::ProcessOnTimerOut()
{
	_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "==============RedisTTimer===========\n" );
	if(redissrv)
	{
		if(redissrv->pingSrv()==0)
			_LOG_DEBUG_( "Ping Redis Server OK\n" );
		else
        {
			_LOG_ERROR_( "Ping Redis Server ERROR, Need ReConnect \n" );
            redissrv->InitRedis();
        }
	}
	else
	{
		_LOG_DEBUG_( "Redis Server DisConnect, Need ReConnect ...\n" );
		redissrv->InitRedis();
	}
	_LOG_DEBUG_( "=======================================\n" );
	StartTimer(60*1000);
	return 0;
}

bool TaskRedis::isUserInGame(int uid)
{
	redisContext *m_redis = NULL;  
    redisReply *reply = NULL;  
	int index = uid % 10;
    m_redis = Util::initRedisContext(Configure::getInstance()->CheckInGame[index].host , Configure::getInstance()->CheckInGame[index].port);
    if (NULL == m_redis)
		return false;
    
	_LOG_INFO_("Initredis-connect OK...\n");
	
	int tid = 0;
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET User:%d tid",uid));
    if(reply == NULL)
    {
        _LOG_ERROR_("Redis [HMGET user:%d status] error\n",uid);
		redisFree(m_redis);
		return false;
    }
	else
	{
		if(reply->type == REDIS_REPLY_STRING)
			tid = atoi(reply->str);
		freeReplyObject(reply);
	}
	redisFree(m_redis);
	if(tid != 0)
		return true;

    return false;
}

