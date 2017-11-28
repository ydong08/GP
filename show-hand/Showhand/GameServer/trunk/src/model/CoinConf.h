#ifndef CoinConf_H
#define CoinConf_H
#include "hiredis.h"
#include "CDL_TCP_Handler.h"
#include "CDL_Timer_Handler.h"
#include "Packet.h"
#include "Player.h"
class Player;
class CoinConf;

class RedisRTimer:public CCTimer
{
	public:
		RedisRTimer(){} ;
		virtual ~RedisRTimer() {};

		inline void init(CoinConf* redissrv){this->redissrv=redissrv;};
private:
		virtual int ProcessOnTimerOut();

		CoinConf* redissrv;
	 		
};

class CoinConf 
{
public:
public:
	static CoinConf* getInstance();
	virtual ~CoinConf();
    int InitRedis();
    int pingSrv();
	int getCoinCfg(CoinCfg * coincfg); 
	bool isUserInBlackList(int uid);
private:
    bool isConnect;
    redisContext *m_redis;                               //Á¬½Óhiredis
    redisReply *reply;                                   //hiredis»Ø¸´
	CoinConf();

    RedisRTimer* heartBeatTimer;
};

#endif

