#include "CoinConf.h"
#include "Logger.h"
#include "Configure.h"
#include "hiredis.h"
#include "Player.h"
#include "Util.h"
#include <vector>
using namespace std;

static CoinConf* instance = NULL;

CoinConf* CoinConf::getInstance()
{
	if(instance==NULL)
	{
		instance = new CoinConf();
	}
	return instance;
}

CoinConf::CoinConf()
{
    heartBeatTimer = new RedisRTimer();
    heartBeatTimer->init(this);
    heartBeatTimer->StartTimer(60*1000);
    m_redis = NULL;
    reply = NULL;
}

CoinConf::~CoinConf()
{
    delete heartBeatTimer;
    if(m_redis)
    {
        redisFree(m_redis);
        m_redis = NULL;
    }
}
int CoinConf::InitRedis()
{
	if(m_redis)
        redisFree(m_redis);
    
	m_redis = Util::initRedisContext(Configure::getInstance()->redis_ip, Configure::getInstance()->redis_port);
	if (NULL == m_redis)
         return -1;
    
	_LOG_INFO_("Initredis-connect OK...\n");
	return 0;
}

int CoinConf::getCoinCfg(CoinCfg * coincfg)
{ 
	if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return -1;
    }
	int ret = 0;
	if(coincfg->level < 4)
		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HMGET ShowHand_RoomConfig:%d raterent minmoney maxmoney retaincoin carrycoin magiccoin playcount1 playcount2 playcount3 roll1 roll2 roll3 roundnum coinhigh1 coinlow1 coinhigh2 coinlow2 coinhigh3 coinlow3 rewardtype rollhigh1 rolllow1 rollhigh2 rolllow2 rollhigh3 rolllow3 ante tax", coincfg->level));
    else
		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HMGET ShowHand_RoomConfig:%d raterent minmoney maxmoney retaincoin carrycoin magiccoin tax", coincfg->level));
	if(reply == NULL)
    {
        _LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
        if(InitRedis() < 0)
            return -1;
        if(coincfg->level < 4)
			reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HMGET ShowHand_RoomConfig:%d raterent minmoney maxmoney retaincoin carrycoin magiccoin playcount1 playcount2 playcount3 roll1 roll2 roll3 roundnum coinhigh1 coinlow1 coinhigh2 coinlow2 coinhigh3 coinlow3 rewardtype rollhigh1 rolllow1 rollhigh2 rolllow2 rollhigh3 rolllow3 ante tax", coincfg->level));
		else
			reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HMGET ShowHand_RoomConfig:%d raterent minmoney maxmoney retaincoin carrycoin magiccoin tax", coincfg->level));
		if(reply == NULL)
            return -1;
    }
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (unsigned int j = 0; j < reply->elements; j++) {
			if(reply->element[j]->type==REDIS_REPLY_NIL)
				continue;
			switch (j)
			{
				case 0: coincfg->raterent = atoi(reply->element[j]->str);break;
				case 1: coincfg->minmoney = atoi(reply->element[j]->str);break;
				case 2: coincfg->maxmoney = atoi(reply->element[j]->str);break;
				case 3: coincfg->retaincoin = atoi(reply->element[j]->str);break;
				case 4: coincfg->carrycoin = atoi(reply->element[j]->str);break;
				case 5: coincfg->magiccoin = atoi(reply->element[j]->str);break;
				case 6: coincfg->playCount1 = atoi(reply->element[j]->str);break;
				case 7: coincfg->playCount2 = atoi(reply->element[j]->str);break;
				case 8: coincfg->playCount3 = atoi(reply->element[j]->str);break;
				case 9: coincfg->getingot1 = atoi(reply->element[j]->str);break;
				case 10: coincfg->getingot2 = atoi(reply->element[j]->str);break;
				case 11: coincfg->getingot3 = atoi(reply->element[j]->str);break;
				case 12: coincfg->roundnum = atoi(reply->element[j]->str);break;
				case 13: coincfg->coinhigh1 = atoi(reply->element[j]->str);break;
				case 14: coincfg->coinlow1 = atoi(reply->element[j]->str);break;
				case 15: coincfg->coinhigh2 = atoi(reply->element[j]->str);break;
				case 16: coincfg->coinlow2 = atoi(reply->element[j]->str);break;
				case 17: coincfg->coinhigh3 = atoi(reply->element[j]->str);break;
				case 18: coincfg->coinlow3 = atoi(reply->element[j]->str);break;
				case 19: coincfg->rewardtype = atoi(reply->element[j]->str);break;
				case 20: coincfg->rollhigh1 = atoi(reply->element[j]->str);break;
				case 21: coincfg->rolllow1 = atoi(reply->element[j]->str);break;
				case 22: coincfg->rollhigh2 = atoi(reply->element[j]->str);break;
				case 23: coincfg->rolllow2 = atoi(reply->element[j]->str);break;
				case 24: coincfg->rollhigh3 = atoi(reply->element[j]->str);break;
				case 25: coincfg->rolllow3 = atoi(reply->element[j]->str);break;
				case 26: coincfg->ante = atoi(reply->element[j]->str); break;
				case 27: coincfg->tax = atoi(reply->element[j]->str); break;
			}				
		}
		if(reply->elements==0){
			_LOG_WARN_("[%s:%d] Can't get configure level[%d],user maybe not in redis\n",__FILE__,__LINE__, coincfg->level);
			ret = -1;
		}
	}else if(reply->type == REDIS_REPLY_ERROR){
		_LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n",__FILE__,__LINE__,reply->type,reply->str);
		ret = -1;
	}
	_LOG_DEBUG_("level[%d], raterent[%d] minmoney[%d] maxmoney[%d] retaincoin[%d] carrycoin[%d] playCount1[%d] playCount2[%d] playCount3[%d] getingot1[%d] getingot2[%d] getingot3[%d] roundnum[%d] coinhigh1[%d] coinlow1[%d] coinhigh2[%d] coinlow2[%d] coinhigh3[%d] coinlow3[%d] rewardtype[%d] rollhigh1[%d] rolllow1[%d] rollhigh2[%d] rolllow2[%d] rollhigh3[%d] rolllow3[%d] magiccoin[%d] \n",
		coincfg->level, coincfg->raterent, coincfg->minmoney, coincfg->maxmoney, coincfg->retaincoin, coincfg->carrycoin, 
		coincfg->playCount1, coincfg->playCount2, coincfg->playCount3, coincfg->getingot1, coincfg->getingot2, coincfg->getingot3,
		coincfg->roundnum, coincfg->coinhigh1, coincfg->coinlow1, coincfg->coinhigh2, coincfg->coinlow2, coincfg->coinhigh3, coincfg->coinlow3,
		coincfg->rewardtype, coincfg->rollhigh1, coincfg->rolllow1, coincfg->rollhigh2, coincfg->rolllow2, coincfg->rollhigh3, coincfg->rolllow3, coincfg->magiccoin);

	freeReplyObject(reply);
	return ret;
}

bool CoinConf::isUserInBlackList(int uid)
{
	if(m_redis == NULL)
    {
        if(InitRedis() < 0)
            return false;
    }

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET ManyBlackList %d", uid));
    if(reply == NULL)
    {
        _LOG_ERROR_("Redis Server is Closed, Can't Connet RedisServer\n");
        if(InitRedis() < 0)
            return false;
        reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET ManyBlackList %d", uid));
        if(reply == NULL)
            return false;
    }
	bool ret = false;
	if(reply->type == REDIS_REPLY_STRING)
	{
		ret = true;
	}

	if(reply)
		freeReplyObject(reply);
	return ret;
}

int CoinConf::pingSrv()
{
    if(m_redis == NULL)
        return -1;
    reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ping"));
    if(reply == NULL)
        return -1;
	freeReplyObject(reply);
    return 0;
}

//=============================RedisRTimer===============================
int RedisRTimer::ProcessOnTimerOut()
{
	_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "==============RedisRTimer===========\n" );
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

