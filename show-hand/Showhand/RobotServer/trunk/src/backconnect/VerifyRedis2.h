#ifndef VerifyRedis2_H
#define VerifyRedis2_H
#include "hiredis.h"
#include "CDL_TCP_Handler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
class VerifyRedis2;
class Table;
class Player;

class Redis2Timer:public CCTimer
{
	public:
		Redis2Timer(){} ;
		virtual ~Redis2Timer() {};

		inline void init(VerifyRedis2* redissrv){this->redissrv=redissrv;};
private:
		virtual int ProcessOnTimerOut();

		VerifyRedis2* redissrv;
	 		
};

class VerifyRedis2 
{
public:
public:
	static VerifyRedis2* getInstance();
	virtual ~VerifyRedis2();
    int InitRedis();
    int pingSrv();

	int getUserNameSex(int &uid, char *nick, short &sex);
private:
    bool isConnect;
    redisContext *m_redis;                               //连接hiredis
    redisReply *reply;                                   //hiredis回复
	redisReply *replyres;                                   //hiredis回复
	VerifyRedis2();

    Redis2Timer* heartBeatTimer;
};

#endif

