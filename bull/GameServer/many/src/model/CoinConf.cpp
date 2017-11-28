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
	if(instance==NULL)
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

	std::string hashKey = StrFormatA("Bull2017_RoomConfig:%d", Configure::getInstance()->m_nLevel);
	vector<string> fields = Util::explode("bankerwincount betwincount starttime endtime betlimit retain upperlimit lowerlimit bankerupperlimit bankerlowerlimit tax bankerlimit bankernumplayer minmoney raise1 raise2 raise3 raise4", " ");
	map<string, string> retVal;
	bool bRet = m_Redis.HMGET(hashKey.c_str(), fields, retVal);
	if (bRet) {
		for (size_t j = 0, max = fields.size(); j < max; ++j) {
			string str_value = retVal[ fields[j] ];
			switch (j)
			{
			case 0: coinCfg.bankerwincount = atoi(str_value.c_str()); break;
			case 1: coinCfg.betwincount = atoi(str_value.c_str()); break;
			case 2: strncpy(coinCfg.starttime, str_value.c_str(), sizeof(coinCfg.starttime)); break;
			case 3: strncpy(coinCfg.endtime, str_value.c_str(), sizeof(coinCfg.endtime)); break;
			case 4: coinCfg.betlimitcoin = atoi(str_value.c_str()); break;
			case 5: coinCfg.retaincoin = atoi(str_value.c_str()); break;
			case 6: coinCfg.upperlimit = atol(str_value.c_str()); break;
			case 7: coinCfg.lowerlimit = atoi(str_value.c_str()); break;
			case 8: coinCfg.bankerupperlimit = atol(str_value.c_str()); break;
			case 9: coinCfg.bankerlowerlimit = atoi(str_value.c_str()); break;
			case 10: coinCfg.tax = atoi(str_value.c_str()); break;
			case 11: coinCfg.bankerlimit = atoi(str_value.c_str()); break;
			case 12: coinCfg.bankernumplayer = atoi(str_value.c_str()); break;
			case 13: coinCfg.minmoney = atoi(str_value.c_str()); break;
			case 14: coinCfg.raise1 = atoi(str_value.c_str()); break;
			case 15: coinCfg.raise2 = atoi(str_value.c_str()); break;
			case 16: coinCfg.raise3 = atoi(str_value.c_str()); break;
			case 17: coinCfg.raise4 = atoi(str_value.c_str()); break;
			}
		}
	}
	else{
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

	LOGGER(E_LOG_INFO) << "getCoinCfg:"
		<< " bankerwincount = " << coinCfg.bankerwincount
		<< " betwincount = " << coinCfg.betwincount
		<< " starttime = " << coinCfg.starttime
		<< " endtime = " << coinCfg.endtime
		<< " limitcoin = " << coinCfg.betlimitcoin
		<< " retaincoin = " << coinCfg.retaincoin
		<< " upperlimit = " << coinCfg.upperlimit
		<< " lowerlimit = " << coinCfg.lowerlimit
		<< " bankerupperlimit = " << coinCfg.bankerupperlimit
		<< " bankerlowerlimit = " << coinCfg.bankerlowerlimit
		<< " tax = " << coinCfg.tax
		<< " bankerlimit = " << coinCfg.bankerlimit
		<< " bankernumplayer = " << coinCfg.bankernumplayer
		<< " minmoney = " << coinCfg.minmoney
		<< " raise1 = " << coinCfg.raise1
		<< " raise2 = " << coinCfg.raise2
		<< " raise3 = " << coinCfg.raise3
		<< " raise4 = " << coinCfg.raise4;

	return bRet;
}

int CoinConf::getWinCount(int64_t &robotwin, int64_t &userbanker)
{
	robotwin = 0;
	userbanker = 0;
	int ret = 0;
	map<string, string> retVal;
	vector<string> fields = Util::explode("robotwin userbanker", " ");
	std::vector<int> wc;  wc.resize(fields.size());
	bool bRet = m_Redis.HMGET("RobotWinManyBull", fields, retVal);
	if (bRet) {
		for (size_t i = 0; i < fields.size(); i++) 
		{
			if (retVal.find(fields[i]) != retVal.end()) 
			{
				wc[i] = atoi(retVal[fields[i]].c_str());
			}
		}
		robotwin = wc[0];
		userbanker = wc[1];
		LOGGER(E_LOG_DEBUG) << "robotwin=" << robotwin << " userbanker=" << userbanker;
	}
	return 0;
}

int CoinConf::setWinCount(int64_t robotwin, int64_t userbanker)
{
	int ret = 0;
	m_Redis.HINCRBY("RobotWinManyBull", "robotwin", robotwin, ret);
	if (userbanker != 0)
	{
		m_Redis.HINCRBY("RobotWinManyBull", "userbanker", userbanker, ret);
	}
	LOGGER(E_LOG_DEBUG) << "robotwin=" << robotwin << " userbanker=" << userbanker;
	return 0;
}

int CoinConf::setTableStatus(short status)
{
	std::string key = StrFormatA("ServerBullTable%d", GameConfigure()->m_nServerId);
	return m_Redis.HSETi(key.c_str(), "status", status);
}       
        
int CoinConf::setTableBanker(int id)
{
	std::string key = StrFormatA("ServerBullTable%d", GameConfigure()->m_nServerId);
	return m_Redis.HSETi(key.c_str(), "banker", id);
}


int CoinConf::setPlayerBet(short type, int64_t betcoin)
{
	std::string key = StrFormatA("ServerBullTable%d", GameConfigure()->m_nServerId);
	std::string buff = StrFormatA("bet%d", type);
	return m_Redis.HSETi(key.c_str(), buff.c_str(), betcoin);
}

int CoinConf::clearPlayerUidBet()
{
	for(int i = 1; i < 5; ++i)
	{
		std::string buff = StrFormatA("ServerBull%d%d", GameConfigure()->m_nServerId, i);
		m_Redis.DEL(buff.c_str());
	}
	return 0;
}

int CoinConf::setPlayerUidBet(short type, int uid, int64_t betcoin)
{
	std::string key = StrFormatA("ServerBull%d%d", GameConfigure()->m_nServerId, type);
	std::string field = StrFormatA("%d", uid);
	return m_Redis.HSETi(key.c_str(), field.c_str(), betcoin);
}

int CoinConf::getSwtichTypeResult()
{
	int ret = 0;
	std::string key = StrFormatA("ServerBullTable%d", GameConfigure()->m_nServerId);
	m_Redis.HGETi(key.c_str(), "result", ret);

	return ret;
}