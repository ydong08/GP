#ifndef CoinConf_H
#define CoinConf_H

#include "BaseCoinConf.h"
#include "Table.h"

class CoinConf : public BaseCoinConf
{
public:
public:
	static CoinConf* getInstance();
	virtual ~CoinConf();
	virtual bool GetCoinConfigData();

	Cfg* getCoinCfg() { return &coinCfg; }

	int getWinCount(int64_t &bankerwin, int64_t &playerwin, int64_t &userbanker); 
	int setWinCount(int64_t bankerwin, int64_t playerwin, int64_t userbanker);
	int delRobotWinInfo();
	int setTableBanker(int id);
	int setTableStatus(short status);
	int setPlayerBet(short type, int64_t betcoin);
	int clearPlayerUidBet();
	int setPlayerUidBet(short type, int uid, int64_t betcoin);
	int getSwtichTypeResult();
	int64_t getJackPot();
	int setJackPot(int64_t money);
	int getjackpotResult(short &jackpotindex, short &jackcardtype);

	int getBlackList(vector<int> &blacklist);
private:
	Cfg coinCfg;

	CoinConf();
};

#endif

