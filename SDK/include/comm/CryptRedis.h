#ifndef __CRYPT_REDIS_H_
#define __CRYPT_REDIS_H_

#include "Util.h"
#include "Typedef.h"
#include <string>
#include "RedisAccess.h"

using namespace std;

class CCryptRedis
{	
	Singleton(CCryptRedis);

public:
	bool init(const char* address);

	bool GetUserKey(uint64_t uid, std::string &strKey);
	bool SetUserKey(uint64_t uid, const std::string &strKey);

private:
	bool connectRedis(const char* address);

private:
	RedisAccess		m_cryptRedis;
};

#endif

