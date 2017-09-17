#ifndef CoinConf_H
#define CoinConf_H

#include "BaseCoinConf.h"
#include "Table.h"

class CoinConf : public BaseCoinConf
{
public:
	static CoinConf* getInstance();
	virtual ~CoinConf();

	virtual bool GetCoinConfigData();

	Cfg* getCoinCfg() { return &coinCfg; }

	int getWinCount(int64_t &bankerwin, int64_t &playerwin, int64_t &userbanker);
	int setWinCount(int64_t bankerwin, int64_t playerwin, int64_t userbanker);
	int setTableBanker(int id);
	int setTableStatus(short status);
	int setPlayerBet(short type, int64_t betcoin);
	int clearPlayerUidBet();
	int setPlayerUidBet(short type, int uid, int64_t betcoin);
	int getSwtichTypeResult();
	int getBlackList(vector<int> &blacklist);
    int getRewardRates(int zhuang_rate , int xian_rate , int ping_rate , int zhuang_dui_rate , int xian_dui_rate);

private:
	CoinConf();

	Cfg coinCfg;
};

#endif

