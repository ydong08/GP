#ifndef VerifyRedis1_H
#define VerifyRedis1_H
#include "hiredis.h"
#include "CDL_TCP_Handler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
class VerifyRedis1;
class Table;
class Player;

class Redis1Timer:public CCTimer
{
	public:
		Redis1Timer(){} ;
		virtual ~Redis1Timer() {};

		inline void init(VerifyRedis1* redissrv){this->redissrv=redissrv;};
private:
		virtual int ProcessOnTimerOut();

		VerifyRedis1* redissrv;
	 		
};

class VerifyRedis1 
{
public:
public:
	static VerifyRedis1* getInstance();
	virtual ~VerifyRedis1();
    int InitRedis();
    int pingSrv();

	int getUserNameSex(int &uid, char *nick, short &sex);
private:
    bool isConnect;
    redisContext *m_redis;                               //连接hiredis
    redisReply *reply;                                   //hiredis回复
	redisReply *replyres;                                   //hiredis回复
	VerifyRedis1();

    Redis1Timer* heartBeatTimer;
};

#endif

