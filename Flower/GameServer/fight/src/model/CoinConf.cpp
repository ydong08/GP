#include "Logger.h"
#include "CoinConf.h"
#include "hiredis.h"
#include "Util.h"
#include "GameApp.h"
#include <vector>
#include "StrFunc.h"
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
	memset(&m_coincfg, 0, sizeof(m_coincfg));
	m_coincfg.level = GameConfigure()->m_nLevel;
	map<string, string> retVal;
	vector<string> fields = Util::explode("minmoney maxmoney ante tax maxlimit maxallin rase1 rase2 "
		"rase3 rase4 magiccoin check_round compare_round", " ");
	bool bRet = m_Redis.HMGET(StrFormatA("Flower_RoomConfig:%d", m_coincfg.level).c_str(), fields, retVal);
	if (bRet) {
		int *pVal[] = { &m_coincfg.minmoney, &m_coincfg.maxmoney, &m_coincfg.ante, &m_coincfg.tax, &m_coincfg.maxlimit, &m_coincfg.maxallin, &m_coincfg.rase1, &m_coincfg.rase2,
			&m_coincfg.rase3, &m_coincfg.rase4, &m_coincfg.magiccoin, &m_coincfg.check_round, &m_coincfg.compare_round };
		for (size_t i = 0; i < fields.size(); i++) {
			if (retVal.find(fields[i]) != retVal.end()) {
				*(pVal[i]) = atoi(retVal[fields[i]].c_str());
			}
		}
	}

	_LOG_DEBUG_("minmoney[%d] maxmoney[%d] ante[%d] tax[%d] maxlimit[%d] maxallin[%d] rase1[%d] rase2[%d] rase3[%d] rase4[%d] magiccoin[%d]\n",
		m_coincfg.minmoney, m_coincfg.maxmoney, m_coincfg.ante, m_coincfg.tax, m_coincfg.maxlimit, m_coincfg.maxallin, m_coincfg.rase1,
		m_coincfg.rase2, m_coincfg.rase3, m_coincfg.rase4, m_coincfg.magiccoin);

	return bRet;
}
