//
// Created by new on 2016/10/19.
//

#ifndef DD_SERVERS_BASECOINCONF_H
#define DD_SERVERS_BASECOINCONF_H

#include <string>
#include "RedisAccess.h"

class BaseCoinConf {

public:
    BaseCoinConf();
    ~BaseCoinConf();

	/// <summary>
	/// 从redis读取coin config数据
	/// </summary>
	/// <returns></returns>
    virtual bool GetCoinConfigData() = 0;
	/// <summary>
	/// 连接coin config redis
	/// </summary>
	/// <returns></returns>
    bool LoadCoinConfig();

protected:
    RedisAccess m_Redis;
};


#endif //DD_SERVERS_BASECOINCONF_H
