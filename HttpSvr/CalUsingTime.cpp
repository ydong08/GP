#include "CalUsingTime.h"
#include <stdio.h>

#include "Logger.h"

CCalUsingTime::CCalUsingTime(std::string strMsg)
{
	gettimeofday(&m_tmBegin, NULL);
	m_strMsg = strMsg;
}


CCalUsingTime::~CCalUsingTime(void)
{
	struct timeval tmEnd;
	gettimeofday(&tmEnd, NULL);
	LOGGER(E_LOG_DEBUG) << m_strMsg << " using time is :" << TimeToInt64(tmEnd) - TimeToInt64(m_tmBegin) << "usec";
}

int64_t CCalUsingTime::TimeToInt64( const struct timeval & tmInput )
{
	return tmInput.tv_sec * 1000000 + tmInput.tv_usec;
}
