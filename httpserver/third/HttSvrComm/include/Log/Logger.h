#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <sstream>
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <stdio.h>
#include <string.h>

#include "Typedef.h"

#include "MyLogger.h"

#define _LOG_REPORT_(uid, opt,  fmt, args...) do{}while(0)
#define _UDP_REPORT_(uid, opt,  fmt, args...) do{}while(0)
#define __LOG_INIT__(level,logfile) 		  do{}while(0)

enum Logger_Level
{
	E_LOG_DEBUG,
	E_LOG_INFO,
	E_LOG_WARNING,
	E_LOG_ERROR,
	E_LOG_FATAL,
	E_LOG_MAX
};

class CLogger
{
//----------static-------------------
public:
	static bool  InitLogger( unsigned char levelLimit, std::string logFile, unsigned short serverId, std::string ip = "", uint16_t port = 0);
	static void Exit();

	static bool  InitUdpClient();

	static void  SendPacket(const char *buff, int length);

	static void* Work(void *p);
public:
	CLogger( unsigned char _log_level, uint32_t uid, const char *file, const char *func, int line);
	~CLogger();

	CLogger& operator<<(int8_t input) {
		m_ss << int32_t(input);
		return *this;
	}

	CLogger& operator<<(uint8_t input) {
		m_ss << uint32_t(input);
		return *this;
	}

	CLogger& operator<<(int16_t input) {
		m_ss << int32_t(input);
		return *this;
	}

	CLogger& operator<<(uint16_t input) {
		m_ss << uint32_t(input);
		return *this;
	}

	template <typename T>
	CLogger& operator<<(T _t) {
		m_ss << _t;
		return *this;
	}

	template <typename T>
	CLogger& operator<<( std::vector<T> _t )
	{
		m_ss << " <Vector:{ ";
		for( typename std::vector<T>::iterator ite = _t.begin(); ite != _t.end(); ++ite )
		{
			m_ss << *ite << " ";
		}
		m_ss << "}> ";
		return *this;
	}

	template <typename T>
	CLogger& operator<<(std::list<T> _t)
	{
		m_ss << " <List:{ ";
		for (typename std::list<T>::iterator ite = _t.begin(); ite != _t.end(); ++ite)
		{
			m_ss << *ite << " ";
		}
		m_ss << "}> ";
		return *this;
	}

	template <typename T>
	CLogger& operator<<(std::deque<T> _t)
	{
		m_ss << " <Deque:{ ";
		for (typename std::deque<T>::iterator ite = _t.begin(); ite != _t.end(); ++ite)
		{
			m_ss << *ite << " ";
		}
		m_ss << "}> ";
		return *this;
	}

	template <typename T>
	CLogger& operator<<(std::set<T> _t)
	{
		m_ss << " <Set:{ ";
		for (typename std::set<T>::iterator ite = _t.begin(); ite != _t.end(); ++ite)
		{
			m_ss << *ite << " ";
		}
		m_ss << "}> ";
		return *this;
	}

	template <typename K, typename V>
	CLogger& operator<<(std::map<K, V> _t)
	{
		m_ss << " <Map:{ ";
		for (typename std::map<K, V>::iterator ite = _t.begin(); ite != _t.end(); ++ite)
		{
			m_ss << "(" << ite->first << " : " << ite->second << ") ";
		}
		m_ss << "}> ";
		return *this;
	}	

	static std::string toHex(int32_t n)
	{
		char buf[64] = { 0 };
		sprintf(buf, "0x%x", n);
		return std::string(buf);
	}

private:
	unsigned char     m_level;
	std::string       m_file_info;
	std::stringstream m_ss;
};

#define TOHEX(i)  CLogger::toHex(i)

//#ifdef WIN32
#	define LOGGER(level) CLogger(level, 0, __FILE__, __FUNCTION__, __LINE__)
#	define ULOGGER(level, uid) CLogger(level, uid, __FILE__, __FUNCTION__, __LINE__)
/*
#else
#	define LOGGER(level) CLogger(level, 0, __FILENAME__, __FUNCTION__, __LINE__)
#	define ULOGGER(level, uid) CLogger(level, uid, __FILENAME__, __FUNCTION__, __LINE__)
#endif
*/

void GetLoggerLine(char* pLine, int nBufLen, const char* format, ...);

#define LOG_LINE_SIZE (5120)
#define _LOG_DEBUG_(fmt, args...) do{char acLine[LOG_LINE_SIZE] = {0}; GetLoggerLine(acLine, LOG_LINE_SIZE, fmt, ##args); LOGGER(E_LOG_DEBUG)<<acLine;}while(0)
#define _LOG_INFO_(fmt, args...)  do{char acLine[LOG_LINE_SIZE] = {0}; GetLoggerLine(acLine, LOG_LINE_SIZE, fmt, ##args); LOGGER(E_LOG_INFO)<<acLine;}while(0)
#define _LOG_WARN_(fmt, args...)  do{char acLine[LOG_LINE_SIZE] = {0}; GetLoggerLine(acLine, LOG_LINE_SIZE, fmt, ##args); LOGGER(E_LOG_WARNING)<<acLine;}while(0)
#define _LOG_ERROR_(fmt, args...) do{char acLine[LOG_LINE_SIZE] = {0}; GetLoggerLine(acLine, LOG_LINE_SIZE, fmt, ##args); LOGGER(E_LOG_ERROR)<<acLine;}while(0)
#define _LOG_TRACK_(fmt, args...) do{char acLine[LOG_LINE_SIZE] = {0}; GetLoggerLine(acLine, LOG_LINE_SIZE, fmt, ##args); LOGGER(E_LOG_FATAL)<<acLine;}while(0)

#endif //__LOGGER_H_
