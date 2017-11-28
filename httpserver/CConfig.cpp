#include "json/json.h"
#include <string.h>
#include <map>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include "Logger.h"
#include "IniFile.h"
#include "CConfig.h"
#include "Util.h"
#include "StrFunc.h"
#include "JsonValueConvert.h"


typedef bool(*pfnGetFile)(const std::string& file, Json::Value& json);
//NOTE:未遍历子目录
static void travel_dir(const char* path, pfnGetFile ongetfile, std::vector<uint32_t>& idv, std::vector<Json::Value>& jv)
{
	DIR* pdir = opendir(path);
	dirent* ent = NULL;
	while ((ent = readdir(pdir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;

		if (ongetfile)
		{
			uint32_t index = atoi(ent->d_name);
			idv.push_back(index);
			std::string abspath = std::string(path) + ent->d_name;
			Json::Value json;
			ongetfile(abspath, json);
			jv.push_back(json);
		}
	}
}

static bool GetConfigFile(const std::string& file, Json::Value& json)
{
	std::ifstream ifs;
	ifs.open(file);
	Json::Reader reader;
	if (!reader.parse(ifs, json, false))
	{
		LOGGER(E_LOG_ERROR) << "open config file failed:" << file;
		ifs.close();
		return false;
	}
	ifs.close();
	return true;
}

static void load_hall_conf(uint32_t pid, const Json::Value &value, std::map<int32_t, std::vector<ip_port_conf> > &hall_conf)
{
	int count = value.size();
	for (int i = 0; i < count; i++)
	{
		Json::Value svrs = value[i];
		int sc = svrs.size();
		std::vector<ip_port_conf> vecs;
		ip_port_conf conf;
		conf.ip = svrs["ip"].asString();
		conf.port = Json::SafeConverToInt32(svrs["port"]);
		vecs.push_back(conf);
		hall_conf[pid] = vecs;
	}
}

static void load_room_conf(int32_t gameid, const Json::Value& value, std::map<int32_t, room_conf>& roomconf)
{
	room_conf rc;
	rc.key = value["key"].asString();
	rc.many = value["many"].asString();
	roomconf[gameid] = rc;
}

bool CConfig::loadFishHallConf(const Json::Value &value, std::vector<ip_port_conf>& conf)
{
    int count = value.size();
    for (int i = 0; i < count; i++)
    {
        ip_port_conf ip;
        ip.ip = value[i]["ip"].asString();
        ip.port = Json::SafeConverToInt32(value[i]["port"]);
        conf.push_back(ip);
    }
    return true;
}

bool CConfig::loadHallConf()
{
	std::string conf = Util::getModulePath() + "/../conf/hall/";
	std::vector<Json::Value> vec;
	std::vector<uint32_t> idv;
	travel_dir(conf.c_str(), GetConfigFile, idv, vec);
	if (idv.empty() || vec.empty() || vec.size() != idv.size())
	{
		LOGGER(E_LOG_ERROR) << "travel config path failed";
		return false;
	}
	for (size_t index = 0; index < idv.size(); index++)
	{
		uint32_t pid = idv[index];
		Json::Value mastersvr = vec[index]["master"]["server"];
		load_hall_conf(pid, mastersvr, master_hall_map);
		const Json::Value& slavesvr = vec[index]["slave"]["server"];
		load_hall_conf(pid, slavesvr, slave_hall_map);
	}
	return true;
}

bool CConfig::loadRoomConf()
{
	std::string conf = Util::getModulePath() + "/../conf/roomconfig/";
	std::vector<Json::Value> vec;
	std::vector<uint32_t> idv;
	travel_dir(conf.c_str(), GetConfigFile, idv, vec);
	if (idv.empty() || vec.empty() || vec.size() != idv.size())
	{
		LOGGER(E_LOG_ERROR) << "travel config path failed";
		return false;
	}
	for (size_t index = 0; index < idv.size(); index++)
	{
		uint32_t gameid = idv[index];
		Json::Value rc = vec[index];
		load_room_conf(gameid, rc, all_room_conf);
	}
	return true;
}

std::string CConfig::GetRedisIpAndPort( int32_t nPos )
{
	if (nPos < 0 || nPos > PORT_CNT) return "";
	return StrFormatA("%s:%d",redis_conf[nPos].ip.c_str(), redis_conf[nPos].port);
}

std::vector<ip_port_conf>* CConfig::getHallConf(int32_t pid)
{
    std::map<int32_t , std::vector<ip_port_conf> >::iterator it = master_hall_map.find(pid);
    if (it != master_hall_map.end())
    {
        return &(it->second);
    }
	//尝试读取../conf/hall目录下对应pid的json
	std::string mp = Util::getModulePath();
	std::string conf = StrFormatA("%s%s%d.json", mp.c_str(), "/../conf/hall/", pid);
	Json::Value json;
	if (GetConfigFile(conf, json))
	{
		load_hall_conf(pid, json["master"]["server"], master_hall_map);
		return &(master_hall_map[pid]);
	}
	it = master_hall_map.find(0); //给出默认大厅
    return &(it->second);
}

std::vector<ip_port_conf>* CConfig::getHallConfBak(int32_t pid)
{
    std::map<int32_t , std::vector<ip_port_conf> >::iterator it = slave_hall_map.find(pid);
    if (it != slave_hall_map.end())
    {
        return &(it->second);
    }
	//尝试读取../conf/hall目录下对应pid的json
	std::string mp = Util::getModulePath();
	std::string conf = StrFormatA("%s%s%d.json", mp.c_str(), "/../conf/hall/", pid);
	Json::Value json;
	if (GetConfigFile(conf, json))
	{
		load_hall_conf(pid, json["slave"]["server"], slave_hall_map);
		return &(slave_hall_map[pid]);
	}
	it = slave_hall_map.find(0); //给出默认大厅
	return &(it->second);
}

std::vector<ip_port_conf>* CConfig::getHallConfFish()
{
    if (fish_hall_vector.size() <= 0)
    {
        return NULL;
    }
    return &fish_hall_vector;
}

bool CConfig::getRoomConfigKey(int gameid, std::string & key, std::string & many)
{
	auto it = all_room_conf.find(gameid);
	if (it != all_room_conf.end())
	{
		key = it->second.key;
		many = it->second.many;
		return true;
	}
	else
	{
		//尝试读取../conf/roomconfig目录下对应gameid的json
		std::string mp = Util::getModulePath();
		std::string conf = StrFormatA("%s%s%d.json", mp.c_str(), "/../conf/roomconfig/", gameid);
		Json::Value json;
		if (GetConfigFile(conf, json))
		{
			load_room_conf(gameid, json, all_room_conf);
			key = all_room_conf[gameid].key;
			many = all_room_conf[gameid].many;
			return true;
		}
	}
	//还是未找到，直接返回gameid为1的
	key = all_room_conf[1].key;
	many = all_room_conf[1].many;
	return false;
}

bool CConfig::Reload()
{
    master_hall_map.clear();
    slave_hall_map.clear();
    fish_hall_vector.clear();
	all_room_conf.clear();
    return LoadConf(m_confFile.c_str());
}

bool CConfig::LoadConf(const char * conf_file)
{
    m_confFile = conf_file;
    std::ifstream ifs;
    ifs.open(conf_file);
	std::cerr<<"Error:"<< strerror(errno) << std::endl;
	std::cout << conf_file << std::endl;
	if (!ifs.is_open())
	{
		std::cout << "Open failed" << std::endl;
	}
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        std::cout << "Open config json failed" << std::endl;
        ifs.close();
        return false;
	}
	ifs.close();

    Json::Value& log = root["log"];
    m_loglevel = Json::SafeConverToInt32(log["level"]);
    m_logRemoteIp = Json::SafeConverToStr(log["ip"]);
    m_logRemotePort = Json::SafeConverToInt32(log["port"],1234);
    
    Json::Value& ucenter = root["ucenter"];
    server_ip = Json::SafeConverToStr(ucenter["ip"], "0.0.0.0");
    server_port = Json::SafeConverToInt32(ucenter["port"], 8085);
    server_id = Json::SafeConverToInt32(ucenter["id"], 1000);
    threadNum = Json::SafeConverToInt32(ucenter["thread_num"],10);
    

	m_jvDbMaster = root["mysql_master"];;
	m_jvDbSlave = root["mysql_slave"];;
	m_jvDbRouting = root["mysql_routing"];;
	//加点日志
	if (m_jvDbMaster.isNull()) LOGGER(E_LOG_WARNING) << "DB Master Setting Is Empty";
	if (m_jvDbSlave.isNull()) LOGGER(E_LOG_WARNING) << "DB Slave Setting Is Empty";
	if (m_jvDbRouting.isNull()) LOGGER(E_LOG_WARNING) << "DB Routing Setting Is Empty";
    
    Json::Value& redis = root["redis"];
    redis_conf[PORT_PHP].ip = Json::SafeConverToStr(redis["php"]["ip"], "127.0.0.1");
    redis_conf[PORT_PHP].port = Json::SafeConverToInt32(redis["php"]["port"], 6379);

    redis_conf[PORT_PARAM].ip = Json::SafeConverToStr(redis["param"]["ip"], "127.0.0.1");
    redis_conf[PORT_PARAM].port = Json::SafeConverToInt32(redis["param"]["port"], 6380);

    redis_conf[PORT_RANK].ip = Json::SafeConverToStr(redis["rank"]["ip"], "127.0.0.1");
    redis_conf[PORT_RANK].port = Json::SafeConverToInt32(redis["rank"]["port"], 6381);

    redis_conf[PORT_OTE].ip = Json::SafeConverToStr(redis["ote"]["ip"], "127.0.0.1");
    redis_conf[PORT_OTE].port = Json::SafeConverToInt32(redis["ote"]["port"], 6382);

    redis_conf[PORT_USER].ip = Json::SafeConverToStr(redis["user"]["ip"], "127.0.0.1");
    redis_conf[PORT_USER].port = Json::SafeConverToInt32(redis["user"]["port"], 6383);

    redis_conf[PORT_UCNF].ip = Json::SafeConverToStr(redis["ucnf"]["ip"], "127.0.0.1");
    redis_conf[PORT_UCNF].port = Json::SafeConverToInt32(redis["ucnf"]["port"], 6384);

    redis_conf[PORT_MON].ip = Json::SafeConverToStr(redis["mon"]["ip"], "127.0.0.1");
    redis_conf[PORT_MON].port = Json::SafeConverToInt32(redis["mon"]["port"], 6385);
    
    Json::Value& money = root["money"];
    m_MoneySvrIp = Json::SafeConverToStr(money["ip"], "192.168.115.70");
    m_MoneySvrPort = Json::SafeConverToInt32(money["port"], 20600);
    m_MoneySvrHeartTime = Json::SafeConverToInt32(money["hearttime"]);
    
    Json::Value& url = root["url"];
    resDomain = url["res_domain"].asString();
    upIcon = url["up_icon"].asString();
    iconDomain = url["icon_domain"].asString();
    feedbackPicDomain = url["feedback_pic_domain"].asString();
    sendmsg = url["send_msg"].asString();
    ucenterUrl = url["ucenter"].asString();
    paycenterUrl = url["paycenter"].asString();
    
    Json::Value& user = root["user"];
    m_UserSvrIp = user["ip"].asString();
    m_UserSvrPort = Json::SafeConverToInt32(user["port"], 20800);
    m_UserSvrHeatTime = Json::SafeConverToInt32(user["hearttime"]);
    
    Json::Value& hall_fish = root["hall"]["fish"];
    loadFishHallConf(hall_fish, fish_hall_vector);
	loadHallConf();
	loadRoomConf();

	m_jvPaymentInfo = root["PaymentInfo"];
    LOGGER(E_LOG_INFO) << root.toStyledString();
    
	Json::Value& maintenance = root["maintenance"];
	m_nMaintenanceType = Json::SafeConverToInt32(maintenance["type"], 0);
	m_strMaintenanceCountry = Json::SafeConverToStr(maintenance["country"]);

    m_SimulatorAllowed = Json::SafeConverToInt32(maintenance["simulator_allowed"], 0);
    m_FakeOnlineConf = root["FakeOnlineConfig"];
}


