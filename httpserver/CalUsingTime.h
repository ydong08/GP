#pragma once

#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <string>

//计算使用时间
class CCalUsingTime
{
public:
	CCalUsingTime(std::string strMsg);
	~CCalUsingTime(void);
private:
	std::string m_strMsg;
	struct timeval m_tmBegin;
	int64_t TimeToInt64(const struct timeval & tmInput);
};

