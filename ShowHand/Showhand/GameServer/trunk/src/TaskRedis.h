#ifndef TaskRedis_H
#define TaskRedis_H
#include "hiredis.h"
#include "CDL_TCP_Handler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
class TaskRedis;
class Table;
class Player;

class RedisTTimer:public CCTimer
{
	public:
		RedisTTimer(){} ;
		virtual ~RedisTTimer() {};

		inline void init(TaskRedis* redissrv){this->redissrv=redissrv;};
private:
		virtual int ProcessOnTimerOut();

		TaskRedis* redissrv;
	 		
};

class TaskRedis 
{
public:
public:
	static TaskRedis* getInstance();
	virtual ~TaskRedis();
    int InitRedis();
    int pingSrv();
	int setPlayerRound(Table* table, Player* player);
	int getCompleteFlag(int uid);
	int getPlayCount(int uid);
	//获取当天完成的局数
	int getPlayerCompleteCount( int uid );
	bool isUserInGame(int uid);
private:
    bool isConnect;
    redisContext *m_redis;                               //连接hiredis
    redisReply *reply;                                   //hiredis回复
	redisReply *replyres;                                   //hiredis回复
	TaskRedis();

    RedisTTimer* heartBeatTimer;
};

#endif

