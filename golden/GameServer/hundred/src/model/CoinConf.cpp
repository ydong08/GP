#include "CoinConf.h"
#include "Logger.h"
#include "Configure.h"
#include "hiredis.h"
#include "Player.h"
#include "Util.h"
#include "GameApp.h"
#include <vector>
#include "StrFunc.h"
using namespace std;


static vector<string> split(const string& src, const string& tok, bool trim, string null_subst)
{
	vector<string> v;
	if (src.empty() || tok.empty())
		return v;
	string::size_type pre_index = 0, index = 0, len = 0;
	while ((index = src.find_first_of(tok, pre_index)) != string::npos)
	{
		if ((len = index - pre_index) != 0)
			v.push_back(src.substr(pre_index, len));
		else if (trim == false)
			v.push_back(null_subst);
		pre_index = index + 1;
	}
	string endstr = src.substr(pre_index);
	if (trim == false) v.push_back(endstr.empty() ? null_subst : endstr);
	else if (!endstr.empty()) v.push_back(endstr);
	return v;
}


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

	map<string, string> retVal;
	vector<string> fields = Util::explode("bankerwincount betwincount starttime endtime limit retain upperlimit lowerlimit bankerupperlimit bankerlowerlimit jackpotswitch tax", " ");
	bool bRet = m_Redis.HMGET("FlowerMany", fields, retVal);
	if (bRet)
	{
		for (size_t idx = 0, max = fields.size(); idx < max; ++idx)
		{
			switch (idx)
			{
				case 0: coinCfg.bankerwincount = atoi(retVal[fields[idx]].c_str()); break;
				case 1: coinCfg.betwincount = atoi(retVal[fields[idx]].c_str()); break;
				case 2: strncpy(coinCfg.starttime, retVal[fields[idx]].c_str(), sizeof(coinCfg.starttime)); break;
				case 3: strncpy(coinCfg.endtime, retVal[fields[idx]].c_str(), sizeof(coinCfg.endtime)); break;
				case 4: coinCfg.limitcoin =  atoi(retVal[fields[idx]].c_str()); break;
				case 5: coinCfg.retaincoin = atoi(retVal[fields[idx]].c_str()); break;
				case 6: coinCfg.upperlimit = atol(retVal[fields[idx]].c_str()); break;
				case 7: coinCfg.lowerlimit = atoi(retVal[fields[idx]].c_str()); break;
				case 8: coinCfg.bankerupperlimit = atol(retVal[fields[idx]].c_str()); break;
				case 9: coinCfg.bankerlowerlimit = atoi(retVal[fields[idx]].c_str()); break;
				case 10: coinCfg.jackpotswitch = atol(retVal[fields[idx]].c_str()); break;
				case 11: coinCfg.tax = atoi(retVal[fields[idx]].c_str()); break;
			}
		}
	}

	vector<std::string> vlist = split(std::string(coinCfg.starttime), ":", true, "");
	if(vlist.size() == 2)
	{
		coinCfg.starthour = atoi(vlist[0].c_str());
		coinCfg.startmin = atoi(vlist[1].c_str());
	}

	vlist = split(std::string(coinCfg.endtime), ":", true, "");
	if(vlist.size() == 2)
	{
		coinCfg.endhour = atoi(vlist[0].c_str());
		coinCfg.endmin = atoi(vlist[1].c_str());
	}
	
	_LOG_DEBUG_("getCoinCfg: bankerwincount=[%d] betwincount=[%d] starttime=[%s] endtime=[%s] limitcoin=[%d] retaincoin=[%d] upperlimit=[%ld] lowerlimit=[%d] bankerupperlimit=[%ld] bankerlowerlimit=[%d] jackpotswitch=[%d]\n",
		coinCfg.bankerwincount, coinCfg.betwincount, coinCfg.starttime, coinCfg.endtime, coinCfg.limitcoin, coinCfg.retaincoin, coinCfg.upperlimit, coinCfg.lowerlimit, coinCfg.bankerupperlimit, coinCfg.bankerlowerlimit, coinCfg.jackpotswitch);
	
	return bRet;
}

int CoinConf::getWinCount(int64_t &bankerwin, int64_t &playerwin, int64_t &userbanker)
{
	bankerwin = 0;
	playerwin = 0;
	userbanker = 0;

	map<string, string> retVal;
	vector<string> fields = Util::explode("bankerwin playerwin userbanker", " ");
	bool bRet = m_Redis.HMGET("RobotWinManyFlower", fields, retVal);
	if (bRet)
	{
		for (size_t idx = 0, max = fields.size(); idx < max; ++idx)
		{
			switch (idx)
			{
				case 0: bankerwin = atol(retVal[fields[idx]].c_str()); break;
				case 1: playerwin = atol(retVal[fields[idx]].c_str()); break;
				case 2: userbanker = atol(retVal[fields[idx]].c_str()); break;
			}
		}
	}

	_LOG_DEBUG_("getWinCount: bankerwin=[%ld] playerwin=[%ld] userbanker=[%ld]\n",
		bankerwin, playerwin, userbanker);

	return bRet ? 0 : -1;
}

int CoinConf::setWinCount(int64_t bankerwin, int64_t playerwin, int64_t userbanker)
{
	int finalVal = 0;
	bool bRet = false;

	if (bankerwin != 0)
	{
		bRet = m_Redis.HINCRBY("HINCRBY", "RobotWinManyFlower", bankerwin, finalVal);
		if (bRet)
		{
			_LOG_DEBUG_("setWinCount, bankerwin: before = [%ld], after = [%d]", bankerwin, finalVal);
		}
	}

	if(playerwin != 0)
	{
		finalVal = 0;
		bRet = false;

		bRet = m_Redis.HINCRBY("HINCRBY", "RobotWinManyFlower", playerwin, finalVal);
		if (bRet)
		{
			_LOG_DEBUG_("setWinCount, playerwin: before = [%ld], after = [%d]", playerwin, finalVal);
		}
	}

	if(userbanker != 0)
	{
		finalVal = 0;
		bRet = false;

		bRet = m_Redis.HINCRBY("HINCRBY", "RobotWinManyFlower", userbanker, finalVal);
		if (bRet)
		{
			_LOG_DEBUG_("setWinCount, userbanker: before = [%ld], after = [%d]", userbanker, finalVal);
		}
	}

	return bRet;
}

int64_t CoinConf::getJackPot()
{
	int64_t finalRet = 0;

	std::string strRetVal;
	bool bRet = m_Redis.HGET(StrFormatA("ServerFlowerTable%d", Configure::getInstance().m_nServerId).c_str(), "jackpot", strRetVal);
	if (bRet)
	{
		finalRet = atol(strRetVal.c_str());
	}

	return finalRet;
}

int CoinConf::setJackPot(int64_t money)
{
	bool bRet = m_Redis.HSET(
		StrFormatA("ServerFlowerTable%d", Configure::getInstance().m_nServerId).c_str(),
		"jackpot",
		StrFormatA("%ld", money).c_str());

	_LOG_DEBUG_("setJackPot: money=[%ld]\n",money);

	return bRet ? 0 : -1;
}

int CoinConf::delRobotWinInfo()
{
	bool bRet = m_Redis.DEL("RobotWinManyFlower");
	return bRet ? 0 : -1;
}

int CoinConf::getBlackList(vector<int> &blacklist)
{
	map<string, string> retVal;
	bool bRet = m_Redis.HGETALL("ManyBlackList", retVal);
	if (bRet)
	{
		int tempIndex = 0;
		map<string, string>::iterator itor = retVal.begin();
		while (itor != retVal.end())
		{
			if (tempIndex % 2 == 0 && tempIndex > 0)
			{
				blacklist.push_back(atoi(itor->second.c_str()));
			}

			++tempIndex;
			++itor;
		}
	}

	return bRet ? 0 : -1;
}

int CoinConf::setTableStatus(short status)
{
	bool bRet = m_Redis.HSETi(StrFormatA("ServerFlowerTable%d", Configure::getInstance().m_nServerId).c_str(), "status", (int)status);
	return bRet ? 0 : -1;
}

int CoinConf::setTableBanker(int id)
{
	bool bRet = m_Redis.HSETi(StrFormatA("ServerFlowerTable%d", Configure::getInstance().m_nServerId).c_str(), "banker", id);
	return bRet ? 0 : -1;
}

int CoinConf::setPlayerBet(short type, int64_t betcoin)
{
	bool bRet = m_Redis.HSET(
		StrFormatA("ServerFlowerTable%d", Configure::getInstance().m_nServerId).c_str(),
		StrFormatA("bet%d", type).c_str(),
		StrFormatA("%ld", betcoin).c_str());
	return bRet ? 0 : -1;
}

int CoinConf::clearPlayerUidBet()
{
	bool bFinalRet = false;
	bool bTempRet = false;

	for(int i = 1; i < 5; ++i)
	{
		bTempRet = m_Redis.DEL(StrFormatA("ServerFlower%d%s", Configure::getInstance().m_nServerId, StrFormatA("bet%d", i).c_str()).c_str());
		bFinalRet = bFinalRet && bTempRet;
	}

	return bFinalRet;
}

int CoinConf::setPlayerUidBet(short type, int uid, int64_t betcoin)
{
	bool bRet = m_Redis.HSET(
		StrFormatA("ServerFlower%d%s", Configure::getInstance().m_nServerId, StrFormatA("bet%d", type).c_str()).c_str(),
		StrFormatA("%d", uid).c_str(),
		StrFormatA("%ld", betcoin).c_str());
	return bRet ? 0 : -1;
}

int CoinConf::getSwtichTypeResult()
{
	int ret = 0;

	std::string strKey = StrFormatA("ServerFlowerTable%d", Configure::getInstance().m_nServerId);

	std::string resultVal;
	bool bRet = m_Redis.HGET(strKey.c_str(), "result", resultVal);
	if (bRet)
	{
		ret = atoi(resultVal.c_str());
	}

	bRet = m_Redis.HSETi(strKey.c_str(), "result", 0);

	return ret;
}

int CoinConf::getjackpotResult(short &jackpotindex, short &jackcardtype)
{
	std::string strKey = StrFormatA("ServerFlowerTable%d", Configure::getInstance().m_nServerId);

	jackpotindex = 0;
	jackcardtype = 0;

	map<string, string> retVal;
	vector<string> fields = Util::explode("jackpotindex jackcardtype", " ");
	bool bRet = m_Redis.HMGET(strKey.c_str(), fields, retVal);
	if (bRet)
	{
		for (size_t idx = 0, max = fields.size(); idx < max; ++idx)
		{
			switch (idx)
			{
			case 0: jackpotindex = atol(retVal[fields[idx]].c_str()); break;
			case 1: jackcardtype = atol(retVal[fields[idx]].c_str()); break;
			}
		}
	}

	_LOG_DEBUG_("getjackpotResult: jackpotindex=[%d] jackcardtype=[%d]\n",
		jackpotindex, jackcardtype);

	retVal.clear();
	retVal["jackpotindex"] = "0";
	retVal["jackcardtype"] = "0";
	bRet = m_Redis.HMSET(strKey.c_str(), retVal);

	return bRet ? 0 : -1;
}
