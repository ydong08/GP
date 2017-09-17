#ifndef CoinConf_H
#define CoinConf_H
#include "BaseCoinConf.h"
#include "Player.h"
class Player;


class CoinConf  : public BaseCoinConf
{
public:
    static CoinCfg sCoinCfg;
public:
	static CoinConf* getInstance();
	virtual ~CoinConf();
	virtual bool GetCoinConfigData();
	int getCoinCfg(CoinCfg * coincfg);

private:
	CoinConf();
};

#endif

