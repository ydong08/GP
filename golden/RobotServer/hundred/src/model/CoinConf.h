#ifndef CoinConf_H
#define CoinConf_H
#include "PlayerManager.h"
#include "RedisAccess.h"

class CoinConf
{
 
	public:
		virtual ~CoinConf(){};
		
		static CoinConf * getInstance();
		int init();
		int initLuaScripts(int i=0);

		void getCoinCfg(Cfg * coincfg); 
	private:
		CoinConf();

		RedisAccess m_Redis;
};



#endif

