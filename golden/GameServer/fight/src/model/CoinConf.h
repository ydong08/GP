#ifndef CoinConf_H
#define CoinConf_H

#include "BaseCoinConf.h"
#include "Player.h"

class CoinConf : public BaseCoinConf
{
public:
public:
	static CoinConf* getInstance();
	virtual ~CoinConf();
	virtual bool GetCoinConfigData();

	CoinCfg* getCoinCfg() { return &m_coincfg; }

private:
	CoinCfg m_coincfg;
    CoinConf();
};

#endif

