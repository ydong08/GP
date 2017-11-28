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
	auto reply = m_Redis.CommandV("HMGET %s bankerwincount betwincount starttime endtime limit retain upperlimit lowerlimit bankerupperlimit bankerlowerlimit jackpotswitch tax",
		room_config_name);
	if (reply == NULL)
	{
		return false;
	}
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (unsigned int j = 0; j < reply->elements; j++) {
			if (reply->element[j]->type == REDIS_REPLY_NIL)
				continue;
			switch (j)
			{
			case 0: coinCfg.bankerwincount = atoi(reply->element[j]->str); break;
			case 1: coinCfg.betwincount = atoi(reply->element[j]->str); break;
			case 2:
				strncpy(coinCfg.starttime, reply->element[j]->str, sizeof(coinCfg.starttime));
				coinCfg.starttime[sizeof(coinCfg.starttime) - 1] = '\0';
				break;
			case 3:
				strncpy(coinCfg.endtime, reply->element[j]->str, sizeof(coinCfg.endtime));
				coinCfg.endtime[sizeof(coinCfg.endtime) - 1] = '\0';
				break;
			case 4: coinCfg.limitcoin = atoi(reply->element[j]->str); break;
			case 5: coinCfg.retaincoin = atoi(reply->element[j]->str); break;
			case 6: coinCfg.upperlimit = atol(reply->element[j]->str); break;
			case 7: coinCfg.lowerlimit = atoi(reply->element[j]->str); break;
			case 8: coinCfg.bankerupperlimit = atol(reply->element[j]->str); break;
			case 9: coinCfg.bankerlowerlimit = atoi(reply->element[j]->str); break;
			case 10: coinCfg.jackpotswitch = atol(reply->element[j]->str); break;
			case 11: coinCfg.tax = atoi(reply->element[j]->str); break;
			}
		}
		if (reply->elements == 0) {
			_LOG_WARN_("[%s:%d] Can't get configure,user maybe not in redis\n", __FILE__, __LINE__);
			return false;
		}
	}
	else if (reply->type == REDIS_REPLY_ERROR) {
		_LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n", __FILE__, __LINE__, reply->type, reply->str);
		return false;
	}
	vector<std::string> vlist = Util::explode(coinCfg.starttime, ":");
	if (vlist.size() == 2)
	{
		coinCfg.starthour = atoi(vlist[0].c_str());
		coinCfg.startmin = atoi(vlist[1].c_str());
	}

	vlist = Util::explode(coinCfg.endtime, ":");
	if (vlist.size() == 2)
	{
		coinCfg.endhour = atoi(vlist[0].c_str());
		coinCfg.endmin = atoi(vlist[1].c_str());
	}

	_LOG_DEBUG_("getCoinCfg: bankerwincount=[%d] betwincount=[%d] starttime=[%s] endtime=[%s] limitcoin=[%d] retaincoin=[%d] upperlimit=[%ld] lowerlimit=[%d] bankerupperlimit=[%ld] bankerlowerlimit=[%d] jackpotswitch=[%ld]\n",
		coinCfg.bankerwincount, coinCfg.betwincount, coinCfg.starttime, coinCfg.endtime, coinCfg.limitcoin, coinCfg.retaincoin, coinCfg.upperlimit, coinCfg.lowerlimit, coinCfg.bankerupperlimit, coinCfg.bankerlowerlimit, coinCfg.jackpotswitch);

	freeReplyObject(reply);
	return true;
}

int CoinConf::getWinCount(int64_t &bankerwin, int64_t &playerwin, int64_t &userbanker)
{
	bankerwin = playerwin = 0;
	int ret = 0;
	map<string, string> retVal;
	vector<string> fields = Util::explode("bankerwin playerwin userbanker", " ");
	std::vector<int> wc;  wc.resize(fields.size());
	bool bRet = m_Redis.HMGET(robot_win_many_name, fields, retVal);
	if (bRet)
	{
		for (size_t i = 0; i < fields.size(); i++)
		{
			if (retVal.find(fields[i]) != retVal.end())
			{
				wc[i] = atoi(retVal[fields[i]].c_str());
			}
		}
		if (fields.size() < 3)
		{
			LOGGER(E_LOG_ERROR) << "getWinCount failed";
			return -1;
		}
		bankerwin = wc[0];
		playerwin = wc[1];
		userbanker = wc[2];
	}
	return 0;
}

int CoinConf::setWinCount(int64_t bankerwin, int64_t playerwin, int64_t userbanker)
{
	int ret = 0;
	m_Redis.HINCRBY(robot_win_many_name, "bankerwin", bankerwin, ret);
	if (playerwin != 0)
	{
		m_Redis.HINCRBY(robot_win_many_name, "playerwin", playerwin, ret);
	}
	if (userbanker != 0)
	{
		m_Redis.HINCRBY(robot_win_many_name, "userbanker", userbanker, ret);
	}
	return 0;
}

int64_t CoinConf::getJackPot()
{
	int ret = 0;
	std::string hash = StrFormatA("ServerTexasTable%d", Configure::getInstance()->m_nServerId);
	if (m_Redis.HGETi(hash.c_str(), "jackpot", ret))
	{
		return ret;
	}
	else
	{
		return 0;
	}
}

int CoinConf::setJackPot(int64_t money)
{
	std::string hash = StrFormatA("ServerTexasTable%d", Configure::getInstance()->m_nServerId);
	m_Redis.HSETi(hash.c_str(), "jackpot", money);
	return 0;
}

int CoinConf::delRobotWinInfo()
{
	m_Redis.DEL(robot_win_many_name);
	return 0;
}

int CoinConf::getBlackList(vector<int> &blacklist)
{
	std::map<std::string, std::string> result;
	if (m_Redis.HGETALL("ManyBlackList", result))
	{
		int index = 0;
		auto it = result.begin();
		while (it != result.end())
		{
			blacklist.push_back(atoi(it->second.c_str()));
			it++;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

int CoinConf::setTableStatus(short status)
{
	std::string key = StrFormatA("ServerTexasTable%d", GameConfigure()->m_nServerId);
	return m_Redis.HSETi(key.c_str(), "status", status);
}       
        
int CoinConf::setTableBanker(int id)
{    
	std::string key = StrFormatA("ServerTexasTable%d", GameConfigure()->m_nServerId);
	return m_Redis.HSETi(key.c_str(), "banker", id);
}


int CoinConf::setPlayerBet(short type, int64_t betcoin)
{
	std::string key = StrFormatA("ServerTexasTable%d", GameConfigure()->m_nServerId);
	std::string buff = StrFormatA("bet%d", type);
	return m_Redis.HSETi(key.c_str(), buff.c_str(), betcoin);
}

int CoinConf::clearPlayerUidBet()
{
	for (int i = 1; i < 5; ++i)
	{
		std::string buff = StrFormatA("ServerTexas%d%d", GameConfigure()->m_nServerId, i);
		m_Redis.DEL(buff.c_str());
	}
	return 0;
}

int CoinConf::setPlayerUidBet(short type, int uid, int64_t betcoin)
{
	std::string key = StrFormatA("ServerTexas%d%d", GameConfigure()->m_nServerId, type);
	std::string field = StrFormatA("%d", uid);
	return m_Redis.HSETi(key.c_str(), field.c_str(), betcoin);
}

int CoinConf::getSwtichTypeResult()
{
	int ret = 0;
	std::string key = StrFormatA("ServerTexasTable%d", GameConfigure()->m_nServerId);
	m_Redis.HGETi(key.c_str(), "result", ret);
	m_Redis.HSETi(key.c_str(), "result", 0);

	return ret;
}

int CoinConf::getjackpotResult(short &jackpotindex, short &jackcardtype)
{
	std::string key = StrFormatA("ServerTexasTable%d", GameConfigure()->m_nServerId);
	std::vector<std::string> fields;
	fields.push_back("jackpotindex");
	fields.push_back("jackcardtype");
	std::map<std::string, std::string> result;
	if (!m_Redis.HMGET(key.c_str(), fields, result))
	{
		return -1;
	}
	jackpotindex = atoi(result["jackpotindex"].c_str());
	jackcardtype = atoi(result["jackcardtype"].c_str());

	result["jackpotindex"] = "0";
	result["jackcardtype"] = "0";
	if (!m_Redis.HMSET(key.c_str(), result))
	{
		return -1;
	}

	return 0;
}