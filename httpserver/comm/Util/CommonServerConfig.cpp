

#include "CommonServerConfig.h"
#include "IniFile.h"
#include "Logger.h"


CommonServerConfig::CommonServerConfig()
{

}

CommonServerConfig::~CommonServerConfig()
{

}

static void get_config(IniFile& ini, const std::string& section, std::string& ip, uint16_t& port, uint16_t& level)
{
	ip = ini.ReadString(section.c_str(), "ip", "");
	port = ini.ReadInt(section.c_str(), "port", 0);
	level = ini.ReadInt(section.c_str(), "level", 0);
}

bool CommonServerConfig::load_config(const std::string& path)
{
	IniFile iniFile(path);
	if (!iniFile.IsOpen())
	{
		LOGGER(E_LOG_ERROR) << "load config failed:" << path;
		return false;
	}

	uint16_t level = 0;
	get_config(iniFile, "dirty", dirty_ip_, dirty_port_, level);
	get_config(iniFile, "log", log_ip_, log_port_, log_level_);
	get_config(iniFile, "round", round_ip_, round_port_, level);
	get_config(iniFile, "trumpt", trumpt_ip_, trumpt_port_, level);
	get_config(iniFile, "mysql", mysql_ip_, mysql_port_, level);
	get_config(iniFile, "user_redis", user_redis_ip_, user_redis_port_, level);
	get_config(iniFile, "server_redis", server_redis_ip_, server_redis_port_, level);
	get_config(iniFile, "operation_redis", operation_redis_ip_, operation_redis_port_, level);

	return true;
}
