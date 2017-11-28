#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include "CoinConf.h"
#include "Logger.h"

using namespace std;
static CoinConf * Instance = NULL;
Cfg CoinConf::sCoinCfg;

CoinConf::CoinConf()
{

}

CoinConf * CoinConf::getInstance()
{
	if(Instance==NULL)
		Instance  = new CoinConf();
	
	return Instance;
}

bool CoinConf::init(const char* addr)
{
	return m_Redis.connect(addr);
}

bool CoinConf::getCoinCfg(Cfg * coincfg)
{
	if(coincfg==NULL)
		return false;
    map<string,string> retVal;
    vector<string> fields = Util::explode("carrynormalhigh carrynormallow normalrobotnum carrybetterhigh carrybetterlow carrybankerhigh carrybankerlow switchbetter bankernum switchbanker bankerwincount betwincount starttime endtime", " ");
	void * pVal[] = {&coincfg->carrynormalhigh, &coincfg->carrynormallow, &coincfg->normalrobotnum, &coincfg->carrybetterhigh, &coincfg->carrybetterlow,
	&coincfg->carrybankerhigh, &coincfg->carrybankerlow, &coincfg->switchbetter, &coincfg->bankernum, &coincfg->switchbanker, &coincfg->bankerwincount,
					 &coincfg->betwincount, coincfg->starttime, coincfg->endtime};
    bool bRet = m_Redis.HMGET("Baccarat_RoomConfig", fields,retVal);
    if (!bRet)
        return false;
	for (size_t i = 0; i < fields.size(); i++) {
		if (retVal.find(fields[i]) != retVal.end()) {
			if (fields[i] == "switchbetter" || fields[i] == "bankernum" || fields[i] == "switchbanker")
				*((BYTE*)pVal[i]) = atoi(retVal[fields[i]].c_str());
			else if (fields[i] == "starttime" || fields[i] == "endtime")
				strcpy((char*)pVal[i], retVal[fields[i]].c_str());
			else if (fields[i] == "normalrobotnum")
				*((short*)pVal[i]) = atoi(retVal[fields[i]].c_str());
			else
				*((int*)pVal[i]) = atoi(retVal[fields[i]].c_str());
		}
	}

	_LOG_DEBUG_("getCoinCfg: bankerwincount=[%d] betwincount=[%d] starttime=[%s] endtime=[%s]\n",
		coincfg->bankerwincount,coincfg->betwincount,coincfg->starttime ,coincfg->endtime);
    return true;
}

