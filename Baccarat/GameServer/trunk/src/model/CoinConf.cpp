#include "CoinConf.h"
#include "Logger.h"
#include "Configure.h"
#include "hiredis.h"
#include "Player.h"
#include "Util.h"
#include "StrFunc.h"
#include "GameApp.h"
#include <vector>
using namespace std;

static CoinConf* instance = NULL;

CoinConf* CoinConf::getInstance()
{
	if (instance == NULL)
	{
		instance = new CoinConf();
	}
	return instance;
}

CoinConf::CoinConf()
{

}

CoinConf::~CoinConf()
{

}

bool CoinConf::GetCoinConfigData()
{
	memset(&coinCfg, 0, sizeof(coinCfg));

	std::string hashKey = StrFormatA("Baccarat2017_RoomConfig:%d", Configure::getInstance()->m_nLevel);
	std::vector<string> fields = Util::explode("bankerwincount betwincount starttime endtime limit retain upperlimit lowerlimit bankerupperlimit bankerlowerlimit tax bankerlimit bankernum betnum minbetnum chip0 chip1 chip2 chip3 personlimitcoin", " ");
	std::map<string, string> retVal;
	bool ret = m_Redis.HMGET(hashKey.c_str(), fields, retVal);
	if (ret) {
		for (unsigned int j = 0, max = fields.size(); j < max; ++j) {
			string str_value = retVal[fields[j]];
			switch (j)
			{
			case 0:  coinCfg.bankerwincount = atoi(str_value.c_str()); break;
			case 1:  coinCfg.betwincount = atoi(str_value.c_str()); break;
			case 2:  strncpy(coinCfg.starttime, str_value.c_str(), sizeof(coinCfg.starttime)); break;
			case 3:  strncpy(coinCfg.endtime, str_value.c_str(), sizeof(coinCfg.endtime)); break;
			case 4:  coinCfg.limitcoin = atoi(str_value.c_str()); break;
			case 5:  coinCfg.retaincoin = atoi(str_value.c_str()); break;
			case 6:  coinCfg.upperlimit = atol(str_value.c_str()); break;
			case 7:  coinCfg.lowerlimit = atoi(str_value.c_str()); break;
			case 8:  coinCfg.bankerupperlimit = atol(str_value.c_str()); break;
			case 9:  coinCfg.bankerlowerlimit = atoi(str_value.c_str()); break;
			case 10: coinCfg.tax = atoi(str_value.c_str()); break;
			case 11: coinCfg.bankerlimit = atoll(str_value.c_str()); break;
			case 12: coinCfg.bankernum = atoi(str_value.c_str()); break;
			case 13: coinCfg.betnum = atoi(str_value.c_str()); break;
			case 14: coinCfg.minbetnum = atoi(str_value.c_str()); break;
			case 15: coinCfg.chiparray[0] = atoi(str_value.c_str()); break;
			case 16: coinCfg.chiparray[1] = atoi(str_value.c_str()); break;
			case 17: coinCfg.chiparray[2] = atoi(str_value.c_str()); break;
			case 18: coinCfg.chiparray[3] = atoi(str_value.c_str()); break;
			case 19: coinCfg.PersonLimitCoin = atoi(str_value.c_str()); break;
			}
		}
	}
	else {
		LOGGER(E_LOG_ERROR) << "Load Redis_Error!";
	}

	vector<std::string> vlist = Util::explode(coinCfg.starttime, ":");
	if (vlist.size() == 2)
	{
		coinCfg.starthour = atoi(vlist[0].c_str());
		coinCfg.startmin = atoi(vlist[1].c_str());
	}

	vlist = Util::explode((coinCfg.endtime), ":");
	if (vlist.size() == 2)
	{
		coinCfg.endhour = atoi(vlist[0].c_str());
		coinCfg.endmin = atoi(vlist[1].c_str());
	}

	_LOG_DEBUG_("getCoinCfg: bankerwincount=[%d] betwincount=[%d] starttime=[%s] endtime=[%s] limitcoin=[%d] retaincoin=[%d] upperlimit=[%ld] lowerlimit=[%d] bankerupperlimit=[%ld] bankerlowerlimit=[%d] personlimitcoin=[%d].\n",
		coinCfg.bankerwincount, coinCfg.betwincount, coinCfg.starttime, coinCfg.endtime, coinCfg.limitcoin, coinCfg.retaincoin, coinCfg.upperlimit, coinCfg.lowerlimit, coinCfg.bankerupperlimit, coinCfg.bankerlowerlimit,
		coinCfg.PersonLimitCoin);

	return ret;
}

int CoinConf::getRewardRates(int zhuang_rate , int xian_rate , int ping_rate , int zhuang_dui_rate , int xian_dui_rate)
{
	std::string hashKey = StrFormatA("Baccarat2017_RoomConfig:%d", Configure::getInstance()->m_nLevel);
	std::vector<string> fields = Util::explode("zhuang_win_rate xian_win_rate ping_rate zhuang_dui_rate xian_dui_rate", " ");
	std::map<string, string> retVal;
	bool ret = m_Redis.HMGET(hashKey.c_str(), fields, retVal);
	
    if (ret)
    {
        for (unsigned int j = 0, max = fields.size(); j < max; ++j)
        {
			string str_value = retVal[fields[j]];
			switch (j)
            {
            case 0: zhuang_rate = atoi(str_value.c_str()); break;
            case 1: xian_rate = atoi(str_value.c_str()); break;
            case 2: ping_rate = atoi(str_value.c_str()); break;
            case 3: zhuang_dui_rate = atoi(str_value.c_str()); break;
            case 4: xian_dui_rate = atoi(str_value.c_str()); break;
            }
        }
    }

    _LOG_DEBUG_("[zhuang_win_rate:%d] [xian_win_rate:%d] [ping_rate:%d] [zhuang_dui_rate:%d] [xian_dui_rate:%d] \n" , zhuang_rate , xian_rate , ping_rate ,  zhuang_dui_rate , xian_dui_rate);

    return ret;
}


int CoinConf::getWinCount(int64_t &bankerwin, int64_t &playerwin, int64_t &userbanker)
{
	bankerwin = 0;
	playerwin = 0;
	userbanker = 0;
	int ret = 0;
	map<string, string> retVal;
	vector<string> fields = Util::explode("bankerwin playerwin userbanker", " ");
	std::vector<int> wc;  wc.resize(fields.size());
	bool bRet = m_Redis.HMGET("RobotWinBaccarat", fields, retVal);
	if (bRet) {
		for (size_t i = 0; i < fields.size(); i++)
		{
			if (retVal.find(fields[i]) != retVal.end())
			{
				wc[i] = atoi(retVal[fields[i]].c_str());
			}
		}
	}
	bankerwin = wc[0];
	playerwin = wc[1];
	userbanker = wc[2];
	LOGGER(E_LOG_DEBUG) << "bankerwin=" << bankerwin << " ,playerwin=" << playerwin << " ,userbanker=" << userbanker;
	return 0;
}

int CoinConf::setWinCount(int64_t bankerwin, int64_t playerwin, int64_t userbanker)
{
	int ret = 0;
	m_Redis.HINCRBY("RobotWinBaccarat", "bankerwin", bankerwin, ret);
	if (playerwin != 0)
	{
		m_Redis.HINCRBY("RobotWinBaccarat", "playerwin", playerwin, ret);
	}
	if (userbanker != 0)
	{
		m_Redis.HINCRBY("RobotWinBaccarat", "userbanker", userbanker, ret);
	}
	LOGGER(E_LOG_DEBUG) << "bankerwin=" << bankerwin << " ,playerwin=" << playerwin << " ,userbanker=" << userbanker;
	return 0;
}

int CoinConf::setTableStatus(short status)
{
	std::string key = StrFormatA("ServerBaccaratTable%d", GameConfigure()->m_nServerId);
	return m_Redis.HSETi(key.c_str(), "status", status);
}

int CoinConf::setTableBanker(int id)
{
	std::string key = StrFormatA("ServerBaccaratTable%d", GameConfigure()->m_nServerId);
	return m_Redis.HSETi(key.c_str(), "banker", id);
}


int CoinConf::setPlayerBet(short type, int64_t betcoin)
{
	std::string key = StrFormatA("ServerBaccaratTable%d", GameConfigure()->m_nServerId);
	std::string buff = StrFormatA("bet%d", type);
	return m_Redis.HSETi(key.c_str(), buff.c_str(), betcoin);
}

int CoinConf::clearPlayerUidBet()
{
	for (int i = 1; i < BETNUM; ++i)
	{
		std::string buff = StrFormatA("ServerBaccarat%d%d", GameConfigure()->m_nServerId, i);
		m_Redis.DEL(buff.c_str());
	}
	return 0;
}

int CoinConf::setPlayerUidBet(short type, int uid, int64_t betcoin)
{
	std::string key = StrFormatA("ServerBaccarat%d%d", GameConfigure()->m_nServerId, type);
	std::string field = StrFormatA("%d", uid);
	return m_Redis.HSETi(key.c_str(), field.c_str(), betcoin);
}

int CoinConf::getSwtichTypeResult()
{
	int ret = 0;
	std::string key = StrFormatA("ServerBaccaratTable%d", GameConfigure()->m_nServerId);
	m_Redis.HGETi(key.c_str(), "result", ret);

	return ret;
}