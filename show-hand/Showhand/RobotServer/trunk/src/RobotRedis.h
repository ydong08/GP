#ifndef RANK_REDIS_H
#define RANK_REDIS_H
#include "hiredis.h"
#include "CDL_TCP_Handler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"

class RobotRedis;

class RedisPingTimer:public CCTimer
{
	public:
		RedisPingTimer(){} ;
		virtual ~RedisPingTimer() {};

		inline void init(RobotRedis* redissrv){this->redissrv=redissrv;};
private:
		virtual int ProcessOnTimerOut();

		RobotRedis* redissrv;
	 		
};

class RobotRedis 
{
public:
public:
	static RobotRedis* getInstance();
	virtual ~RobotRedis();
    int InitRedis();
	std::string readHeadUrl();
    int64_t getRobotWinCount(short level);
	int AddRobotWin(short level, int64_t money);
    int pingSrv();
private:
    bool isConnect;
    redisContext *m_redis;                               //连接hiredis
    redisReply *reply;                                   //hiredis回复
	RobotRedis();

    RedisPingTimer* heartBeatTimer;
};

#endif

