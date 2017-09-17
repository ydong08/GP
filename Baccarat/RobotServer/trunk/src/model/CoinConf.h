#ifndef CoinConf_H
#define CoinConf_H
#include "PlayerManager.h"
#include "RedisAccess.h"

class CoinConf
{
 
	public:
		virtual ~CoinConf(){};

		static Cfg sCoinCfg;
		static CoinConf * getInstance();
		bool init(const char* addr);

		bool getCoinCfg(Cfg * coincfg);
	private:
		CoinConf();
		RedisAccess m_Redis;
};



#endif

