
#include <fstream>
#include <string.h>
#include <stdarg.h>

#ifndef WIN32
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "Typedef.h"
#include "Logger.h"
#include "Console.h"
#include "MsgQueue.h"
#include "Util.h"

#define TIMESEP_LOGGER_FILE_CHANGE 86400

const char* __LoggerString[E_LOG_MAX] = { "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

namespace LogSpace
{
	bool						 gbInit = false;
	bool						 m_bLogWork = false;
	bool						 gNeedRefrashLogFile;
	unsigned long				 gLastLogFileTime_S;
	std::fstream				 gLogStream;
	std::string					 gLogFileFullPath;
	std::string					 gAppName;
	unsigned char				 gLogLevel  = 0;
	std::string					 gLocalIP;
	unsigned short				 gServerID;
	stdx::msg_queue<std::string> gLogQueue;	
	bool						 gbRemote;
	struct sockaddr_in           gAddrInfo;
	int							 gAddrLength;
	int							 gNetfd;
};

using namespace LogSpace;

CLogger::CLogger( unsigned char _log_level, uint32_t uid, const char *file, const char *func, int line)
	: m_level( _log_level )
{
	const char *curFile = strrchr(file, '/');
	if (curFile == NULL) {
		curFile = file;
	} else {
		curFile++;
	}
	long temp_s = Util::getGMTTime_S();
	if (  TIMESEP_LOGGER_FILE_CHANGE <= temp_s - gLastLogFileTime_S  && !gNeedRefrashLogFile )
	{
		gNeedRefrashLogFile = true;
	}
	m_ss << gLocalIP << ":" << gAppName << " | " << __LoggerString[_log_level] << " | " 
		 << Util::formatGMTTime() << " | " << curFile << ":" << func << ":" << line << ":" << uid << " | ";
}

CLogger::~CLogger()
{
	if ( m_ss.str().length() == 0)
	{
		return;
	}

	if ( m_level < gLogLevel )
	{
		return;
	}

	if (gbInit)
	{
		gLogQueue.push_back( std::string(m_ss.str().c_str()));
	}

	switch( m_level )
	{
	case E_LOG_DEBUG:
		STDX_LOG_DEBUG( m_ss.str() );
		break;
	case E_LOG_INFO:
		STDX_LOG_INFO( m_ss.str() );
		break;
	case E_LOG_WARNING:
		STDX_LOG_WARNING( m_ss.str() );
		break;
	case E_LOG_ERROR:
		STDX_LOG_ERROR( m_ss.str() );
		break;
	case E_LOG_FATAL:
		STDX_LOG_CRIT( m_ss.str() );
		break;
	
	default:
		STDX_LOG_INFO( m_ss.str() );
		break;
	}
}

//===============================static function===================================
bool CLogger::InitLogger( unsigned char levelLimit, std::string logFile, unsigned short serverId, std::string ip, unsigned short port)
{
	unsigned long temp_s = Util::getGMTTime_S();
	gLogLevel = levelLimit;
	gLogFileFullPath = "../log/";
	char serverBuff[64] = { 0 };
	snprintf(serverBuff, sizeof(serverBuff), "_%05d", serverId);
	gAppName = logFile + std::string(serverBuff);
	gLogFileFullPath += gAppName + "_" + Util::formatGMTTime_Date() + ".log";
	gNeedRefrashLogFile = false;
	gServerID = serverId;
	gLastLogFileTime_S = temp_s / TIMESEP_LOGGER_FILE_CHANGE * TIMESEP_LOGGER_FILE_CHANGE + Util::getTimeDifferenceS();
	
	Util::getLocalIp(gLocalIP);
	gbRemote = false;
	if (!ip.empty() && port != 0) {
		bzero(&gAddrInfo, sizeof(gAddrInfo));  
		gAddrInfo.sin_family = AF_INET;  
		gAddrInfo.sin_port = htons(port);  
		gAddrInfo.sin_addr.s_addr = inet_addr(ip.c_str());
		gAddrLength = sizeof(struct sockaddr_in);
		gbRemote = true;

		if (!InitUdpClient()) {
			return false;
		}
	}
	
	Util::mkdir("../log/");
	gLogStream.open(gLogFileFullPath.c_str(), std::ios::out | std::ios::app);
	if (!gLogStream.is_open() || !gLogStream.good()) {
		return false;
	}

	pthread_t m_tid;
	int ret = pthread_create(&m_tid, 0, CLogger::Work, NULL);
	if (ret ==  0) {
		return true;
	}
	pthread_detach(m_tid);
	return false;
}

void CLogger::Exit()
{
	m_bLogWork = false;
}

bool CLogger::InitUdpClient()
{
	gNetfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (gNetfd < 0) {  
		perror("InitUdpClient");  
		return false;  
	}  
	return true;
}

void CLogger::SendPacket(const char *buff, int length)
{
	::sendto(gNetfd, buff, length, 0, (struct sockaddr *)&gAddrInfo, gAddrLength);
}

void* CLogger::Work(void *p)
{
	gbInit = true;
	m_bLogWork = true;
	std::string tempOut;
	while(m_bLogWork)
	{
		tempOut = gLogQueue.wait();

		if ( gNeedRefrashLogFile ) {
			unsigned long temp_s = Util::getGMTTime_S();
			gLastLogFileTime_S = temp_s / TIMESEP_LOGGER_FILE_CHANGE * TIMESEP_LOGGER_FILE_CHANGE + Util::getTimeDifferenceS();
			gNeedRefrashLogFile = false;
			gLogStream.close();
			gLogFileFullPath = "../log/";
			gLogFileFullPath += gAppName + "_" + Util::formatGMTTime_Date() + ".log";
			gLogStream.open( gLogFileFullPath.c_str(), std::ios::out | std::ios::app );
		}
		
		if (gbRemote) {
			SendPacket(tempOut.c_str(), tempOut.size());
		}

		if ( gLogStream.is_open() && gLogStream.good() ) {
			gLogStream << tempOut << std::endl;
		} else {
			gNeedRefrashLogFile = true;
			//TODO:MAKE ERROR INFO
		}
	}
	pthread_exit(NULL);
	return NULL;
}

void GetLoggerLine(char* pLine, int nBufLen, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int n = vsnprintf(pLine, nBufLen, format, ap);
	va_end(ap);

	if (n<=0)
	{
		pLine[0] = '\0';
	}
	else
	{ 
		if ('\n' == pLine[n - 1])
			pLine[n - 1] = '\0';
		else
			pLine[n] = '\0';
	}
}