#include "CoinConf.h"
#include "GameApp.h"
#include "Util.h"
#include "StrFunc.h"
#include "Logger.h"
using namespace std;

static CoinConf* instance = NULL;
CoinCfg CoinConf::sCoinCfg;

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
    CoinCfg *coincfg = &sCoinCfg;
    coincfg->level = GameConfigure()->m_nLevel;
    map<string, string> retVal;
    vector<string> fields = Util::explode("minmoney maxmoney tax retaincoin carrycoin bigblind carrycoinmax maxnum magiccoin rateante ratetax mustbetcoin", " ");
    bool bRet = m_Redis.HMGET(StrFormatA("Texas_RoomConfig:%d", coincfg->level).c_str(), fields, retVal);
    if (bRet) {
        int *pVal[] = {&coincfg->minmoney, &coincfg->maxmoney, &coincfg->tax, &coincfg->retaincoin, &coincfg->carrycoin,
                       &coincfg->bigblind, &coincfg->carrycoinmax, &coincfg->maxnum,
                       &coincfg->magiccoin, &coincfg->rateante, &coincfg->ratetax, &coincfg->mustbetcoin};
        for (size_t i = 0; i < fields.size(); i++) {
            if (retVal.find(fields[i]) != retVal.end()) {
                *(pVal[i]) = atoi(retVal[fields[i]].c_str());
            }
        }
    }

    _LOG_DEBUG_("level[%d] minmoney[%d] maxmoney[%d] tax[%d] retaincoin[%d] carrycoin[%d] bigblind[%d] carrycoinmax[%d] maxnum[%d] magiccoin[%d] rateante[%d] ratetax[%d] mustbetcoin[%d]\n",
                coincfg->level, coincfg->minmoney, coincfg->maxmoney, coincfg->tax, coincfg->retaincoin, coincfg->carrycoin, coincfg->bigblind, coincfg->carrycoinmax,
                coincfg->maxnum, coincfg->magiccoin, coincfg->rateante, coincfg->ratetax, coincfg->mustbetcoin);

    return bRet;
}

int CoinConf::getCoinCfg(CoinCfg * coincfg)
{
    memcpy(coincfg, &sCoinCfg, sizeof(CoinCfg));

    return 0;
}


