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
	bool ret = true;
	auto reply = m_Redis.CommandV("HMGET Baccarat_RoomConfig bankerwincount betwincount starttime endtime limit retain upperlimit lowerlimit bankerupperlimit bankerlowerlimit tax bankerlimit bankernum betnum minbetnum chip0 chip1 chip2 chip3 chip4 chip5 personlimitcoin");
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (unsigned int j = 0; j < reply->elements; j++) {
			if (reply->element[j]->type == REDIS_REPLY_NIL)
				continue;
			switch (j)
			{
			case 0:  coinCfg.bankerwincount = atoi(reply->element[j]->str); break;
			case 1:  coinCfg.betwincount = atoi(reply->element[j]->str); break;
			case 2:  strncpy(coinCfg.starttime, reply->element[j]->str, sizeof(coinCfg.starttime)); break;
			case 3:  strncpy(coinCfg.endtime, reply->element[j]->str, sizeof(coinCfg.endtime)); break;
			case 4:  coinCfg.limitcoin = atoi(reply->element[j]->str); break;
			case 5:  coinCfg.retaincoin = atoi(reply->element[j]->str); break;
			case 6:  coinCfg.upperlimit = atol(reply->element[j]->str); break;
			case 7:  coinCfg.lowerlimit = atoi(reply->element[j]->str); break;
			case 8:  coinCfg.bankerupperlimit = atol(reply->element[j]->str); break;
			case 9:  coinCfg.bankerlowerlimit = atoi(reply->element[j]->str); break;
			case 10: coinCfg.tax = atoi(reply->element[j]->str); break;
			case 11: coinCfg.bankerlimit = atoll(reply->element[j]->str); break;
			case 12: coinCfg.bankernum = atoi(reply->element[j]->str); break;
			case 13: coinCfg.betnum = atoi(reply->element[j]->str); break;
			case 14: coinCfg.minbetnum = atoi(reply->element[j]->str); break;
			case 15: coinCfg.chiparray[0] = atoi(reply->element[j]->str); break;
			case 16: coinCfg.chiparray[1] = atoi(reply->element[j]->str); break;
			case 17: coinCfg.chiparray[2] = atoi(reply->element[j]->str); break;
			case 18: coinCfg.chiparray[3] = atoi(reply->element[j]->str); break;
			case 19: coinCfg.chiparray[4] = atoi(reply->element[j]->str); break;
			case 20: coinCfg.chiparray[5] = atoi(reply->element[j]->str); break;
			case 21: coinCfg.PersonLimitCoin = atoi(reply->element[j]->str); break;
			}
		}
		if (reply->elements == 0) {
			_LOG_WARN_("[%s:%d] Can't get configure,user maybe not in redis\n", __FILE__, __LINE__);
			ret = false;
		}
	}
	else if (reply->type == REDIS_REPLY_ERROR) {
		_LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n", __FILE__, __LINE__, reply->type, reply->str);
		ret = false;
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

	freeReplyObject(reply);
	return ret;
}

int CoinConf::getRewardRates(int zhuang_rate , int xian_rate , int ping_rate , int zhuang_dui_rate , int xian_dui_rate)
{
    bool ret = true;
    auto reply = m_Redis.CommandV("hmget Baccarat_RoomConfig zhuang_win_rate xian_win_rate ping_rate zhuang_dui_rate xian_dui_rate");
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (unsigned int j = 0; j < reply->elements; j++)
        {
            if (reply->element[j]->type == REDIS_REPLY_NIL)
                continue;
            switch (j)
            {
            case 0: zhuang_rate = atoi(reply->element[j]->str); break;
            case 1: xian_rate = atoi(reply->element[j]->str); break;
            case 2: ping_rate = atoi(reply->element[j]->str); break;
            case 3: zhuang_dui_rate = atoi(reply->element[j]->str); break;
            case 4: xian_dui_rate = atoi(reply->element[j]->str); break;
            }
        }
        if (reply->elements == 0)
        {
            _LOG_WARN_("hmget Baccarat_RoomConfig zhuang_win_rate xian_win_rate ping_rate zhuang_dui_rate xian_dui_rate  failed !!!\n" );
            ret = false;
        }
    }
    else if (reply->type == REDIS_REPLY_ERROR)
    {
        _LOG_ERROR_("Redis_Error[%d][%s]\n" ,  reply->type , reply->str);
        ret = false;
    }

    _LOG_DEBUG_("[zhuang_win_rate:%d] [xian_win_rate:%d] [ping_rate:%d] [zhuang_dui_rate:%d] [xian_dui_rate:%d] \n" , zhuang_rate , xian_rate , ping_rate ,  zhuang_dui_rate , xian_dui_rate);

    freeReplyObject(reply);
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