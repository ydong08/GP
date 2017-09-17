#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "CoinConf.h"
#include "Logger.h"
#include "Configure.h"
#include "StrFunc.h"


using namespace std;
static CoinConf * Instance = NULL;


CoinConf::CoinConf()
{

}

CoinConf * CoinConf::getInstance()
{
	if(Instance==NULL)
		Instance  = new CoinConf();
	
	return Instance;
}

int CoinConf::init()
{
	m_Redis.connect(Configure::getInstance()->m_sServerRedisAddr[READ_REDIS_CONF].c_str());
	return 0;
}

void CoinConf::getCoinCfg(Cfg * coincfg)
{
	if (coincfg == NULL)
		return;

	memset(coincfg, 0, sizeof(Cfg));

	std::vector<std::string> fields = {
		"carrynormalhigh", "carrynormallow","normalrobotnum", 
		"carrybetterhigh","carrybetterlow", "carrybankerhigh",
		"carrybankerlow","switchbetter", "bankernumrobot","switchbanker",
		"bankerwincount","betwincount","starttime", "endtime"
	};
	std::map<std::string, std::string> result;
	if (!m_Redis.HMGET( StrFormatA("Bull2017_RoomConfig:%d", Configure::getInstance()->m_nLevel).c_str(), fields, result) )
	{
		LOGGER(E_LOG_ERROR) << "get data from redis failed";
		return;
	}
	coincfg->carrynormalhigh = atoi(result["carrynormalhigh"].c_str());
	coincfg->carrynormallow = atoi(result["carrynormallow"].c_str());
	coincfg->normalrobotnum = atoi(result["normalrobotnum"].c_str());
	coincfg->carrybetterhigh = atoi(result["carrybetterhigh"].c_str());
	coincfg->carrybetterlow = atoi(result["carrybetterlow"].c_str());
	coincfg->carrybankerhigh = atoi(result["carrybankerhigh"].c_str());

	coincfg->carrybankerlow = atoi(result["carrybankerlow"].c_str());
	coincfg->switchbetter = atoi(result["switchbetter"].c_str());
	coincfg->bankernum = atoi(result["bankernumrobot"].c_str());
	coincfg->switchbanker = atoi(result["switchbanker"].c_str());

	coincfg->bankerwincount = atoi(result["bankerwincount"].c_str());
	coincfg->betwincount = atoi(result["betwincount"].c_str());
	sprintf(coincfg->starttime, "%s", result["starttime"].c_str());
	sprintf(coincfg->endtime, "%s", result["endtime"].c_str());
}


