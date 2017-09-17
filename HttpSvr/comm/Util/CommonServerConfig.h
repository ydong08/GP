

#pragma once

#include <string>
#include "Typedef.h"

class CommonServerConfig
{
	Singleton(CommonServerConfig);

public:
	bool load_config(const std::string& path);

public:
	std::string log_ip_;
	uint16_t log_port_;
	uint16_t log_level_;

	std::string round_ip_;
	uint16_t round_port_;

	std::string trumpt_ip_;
	uint16_t trumpt_port_;

	std::string dirty_ip_;
	uint16_t dirty_port_;

	std::string mysql_ip_;
	uint16_t mysql_port_;

	std::string user_redis_ip_;
	uint16_t user_redis_port_;

	std::string server_redis_ip_;
	uint16_t server_redis_port_;

	std::string operation_redis_ip_;
	uint16_t operation_redis_port_;
};