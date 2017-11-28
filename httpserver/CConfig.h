#ifndef _SYSTEM_CONFIG_H_
#define _SYSTEM_CONFIG_H_

#include <iostream>
#include <map>
#include <vector>
#include "json/json.h"

/**************************************************************************************
### redis端口说明 ###
redis部署在116.31.115.73服务器上
* 6379 php使用
* 6380 服务器配置参数存储，部分临时数据存储，如离线玩家记录(需要移走)，游戏人数记录等
* 6381 C++服务器写入，php读取，游戏排行榜统计
* 6382 OTE字段，C++服务器写入，php读取
* 6383 USER字段，C++服务器写入，php读取
* 6384 UCNF字段，C++服务器写入，php读取
* 6385 MON字段，C++服务器写入，php读取
***************************************************************************************/

struct ip_port_conf
{
    std::string ip;
    int32_t port;
};

struct room_conf
{
	std::string key;
	std::string many;
};

namespace Json
{
    class Value;
}

enum REDIS_PORT
{
    PORT_PHP = 0,
    PORT_PARAM,
    PORT_RANK,
    PORT_OTE,
    PORT_USER,
    PORT_UCNF,
    PORT_MON,
    PORT_CNT
};

class CConfig
{
public:
	virtual ~CConfig(){	}
	bool LoadConf(const char * conf_file);
    std::vector<ip_port_conf>* getHallConf(int32_t pid);
    std::vector<ip_port_conf>* getHallConfBak(int32_t pid);
    std::vector<ip_port_conf>* getHallConfFish();
	bool getRoomConfigKey(int gameid, std::string& key, std::string& many);
    
    bool Reload();

	static CConfig& getInstance()
	{
		static CConfig inst;
		return inst;
	}

protected:
    CConfig() : threadNum(10) {}
    
    bool loadFishHallConf(const Json::Value& value, std::vector<ip_port_conf>& conf);
	bool loadHallConf();
	bool loadRoomConf();

private:
	std::string m_confFile;
	
public:
    int server_id;
    
    std::string server_ip;
    int server_port;
    
	Json::Value	m_jvDbMaster;
	Json::Value	m_jvDbSlave;
	Json::Value	m_jvDbRouting;
/*	std::string db_ip;
	int db_port;
	std::string db_user;
	std::string db_passwd;*/

//	std::string redis_ip;
/*	int redis_php_port;
    int redis_param_port;
	int redis_rank_port;
	int redis_OTE_port;
	int redis_USER_port;
	int redis_UCNF_port;
	int redis_MON_port;*/
    
    ip_port_conf redis_conf[PORT_CNT];
	std::string GetRedisIpAndPort(int32_t nPos);
	
	std::string m_MoneySvrIp;
	int m_MoneySvrPort;
	int m_MoneySvrHeartTime;
    
    std::string m_UserSvrIp;
    int m_UserSvrPort;
    int m_UserSvrHeatTime;

	std::string resDomain;
	std::string upIcon;
	std::string iconDomain;
	std::string feedbackPicDomain;
	std::string sendmsg;

    std::string ucenterUrl;
    std::string paycenterUrl;

    std::map<int32_t, std::vector<ip_port_conf> > master_hall_map;
    std::map<int32_t , std::vector<ip_port_conf> > slave_hall_map;
    std::vector<ip_port_conf>  fish_hall_vector;
	std::map<int32_t, room_conf> all_room_conf;
    
    //日志
    int			  m_loglevel;
    std::string   m_logRemoteIp;
    UINT16        m_logRemotePort;
    
    //
    int            threadNum;
	Json::Value	m_jvPaymentInfo;

	char m_nMaintenanceType;
	std::string m_strMaintenanceCountry;
    int            m_SimulatorAllowed;
    Json::Value    m_FakeOnlineConf;

};


#endif // !_SYSTEM_CONFIG_H_
