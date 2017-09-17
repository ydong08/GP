#include <sys/time.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <assert.h>
#include "Logger.h"
#include "Util.h"
//#include "Protocol.h"

static std::string strPoll = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";

void Util::registerSignal()
{
	signal(SIGHUP,  SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
}

void Util::daemonize()
{
#ifndef WIN32
	pid_t pid, sid;

	/* already a daemon */
	if (getppid() == 1) return;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* At this point we are executing as the child process */

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Redirect standard files to /dev/null */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);
#endif 
}

int Util::getGMTTime_Y()
{
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	return (timeinfo->tm_year +1900);
}

long Util::getGMTTime_S()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec;
}

long Util::getGMTTime_U()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec;
}

std::string Util::formatGMTTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	time_t now = tv.tv_sec;
	struct tm timeinfo;
	char buf[128];
	localtime_r(&now, &timeinfo);
	bzero(buf, 128);
	snprintf(buf, sizeof(buf), "%4d-%02d-%02d-%02d:%02d:%02d:%08ld", 1900 + timeinfo.tm_year, 1 + timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, tv.tv_usec);
	return buf;
}

std::string Util::formatGMTTimeS()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    time_t now = tv.tv_sec;
    struct tm timeinfo;
    char buf[128];
    localtime_r(&now, &timeinfo);
    bzero(buf, 128);
    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d", 1900 + timeinfo.tm_year, 1 + timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    return buf;
}

std::string Util::formatGMTTime_Date()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	time_t now = tv.tv_sec;
	struct tm timeinfo;
	char buf[128];

	localtime_r(&now, &timeinfo);
	bzero(buf, 128);
	snprintf(buf, sizeof(buf), "%4d-%02d-%02d", 1900 + timeinfo.tm_year, 1 + timeinfo.tm_mon, timeinfo.tm_mday);
	return buf;
}

int Util::getTimeDifferenceS()
{
	struct timeval tv;
	struct timezone tz;
	if (gettimeofday(&tv, &tz))
		return 0;

	return tz.tz_minuteswest * 60;
}

bool Util::isSameDay(const time_t time1, const time_t time2)
{
    struct tm * tm1 = gmtime(&time1);
    struct tm * tm2 = gmtime(&time2);

    if (tm1->tm_year == tm2->tm_yday &&
        tm1->tm_mon == tm2->tm_mon &&
        tm1->tm_mday == tm2->tm_mday)
    {
        return true;
    }

    return false;
}

bool Util::isToday(const time_t time1)
{
    time_t time2 = time(NULL);

    return isSameDay(time1 , time2);
}

bool Util::getLocalIp(std::string &str)
{
	struct ifaddrs * ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);
	struct ifaddrs * ifAddrStructHead = ifAddrStruct;
	while (ifAddrStruct != NULL)
	{
		if (ifAddrStruct->ifa_addr->sa_family == AF_INET)
		{ // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			if (strcmp(ifAddrStruct->ifa_name, "eth0") == 0)
			{
				str = addressBuffer;
			}
			else if (strcmp(ifAddrStruct->ifa_name, "em1") == 0) 
			{
				str = addressBuffer;
			}
			//printf("%s IP Address %s/n", ifAddrStruct->ifa_name, addressBuffer); 
		}
		else if (ifAddrStruct->ifa_addr->sa_family == AF_INET6)
		{ // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			//printf("%s IP Address %s/n", ifAddrStruct->ifa_name, addressBuffer); 
		}
		ifAddrStruct = ifAddrStruct->ifa_next;
	}
	if(ifAddrStructHead) freeifaddrs(ifAddrStructHead);
	return true;
}

bool Util::dir_exist(const std::string& name)
{
	assert(!name.empty());
	struct stat st;
	return (::stat(name.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
}

template <typename _Tp, typename _CharT>
const _Tp& Util::split_path(std::basic_string<_CharT> path, _CharT delim, _Tp& entries)
{
	typedef typename std::basic_string<_CharT> string_type;
	if (path.empty())
		return entries;
	path.erase(path.find_last_not_of("/") + 1);
	typename string_type::size_type pos = 0;
	do
	{
		pos = path.find('/', pos + 1);
		entries.push_back(path.substr(0, pos));
	} while (pos != string_type::npos);
	return entries;
}

bool Util::mkdir(const std::string& path)
{
	if (path.empty())
		return false;
	if (dir_exist(path))
		return true;

	std::vector<std::string> entries;
	split_path(path, '/', entries);
	for (std::vector<std::string>::iterator it = entries.begin();
		it != entries.end(); ++it)
	{
		if (!dir_exist(*it))
		{
			if (::mkdir(it->c_str(), S_IRWXU) == -1)
				return false;
		}
	}

	return true;
}

bool Util::makeRobotNameSex(std::string headlink, std::string &name, int &sex, std::string &headurl)
{
	sex = (rand() % 2) + 1;
	int randNum = rand() % 500;
	if (randNum < 100)
	{
		char buffer[256] = { 0 };
		int index = rand() % 50 + 1;
		if (sex == 1) { //men
			sprintf(buffer, "%s/men/%d.jpg", headlink.c_str(), index);
		}
		else { //women
			sprintf(buffer, "%s/women/%d.jpg", headlink.c_str(), index);
		}
		headurl = buffer;
	}
	else
	{
		headurl = ""; //is null, client use default.
	}

	for (int i = 0; i < 12; i++)
	{
		name += strPoll[rand()%strPoll.size()];
	}	
	return true;
}

bool Util::makeRobotInfo(std::string headlink, int &sex, std::string &headurl, int &uid)
{
	sex = (rand() % 2) + 1;
	int randNum = rand() % 500;
	if (randNum < 400)
	{
		char buffer[256] = { 0 };
		int index = rand() % 50 + 1;
		if (sex == 1) { //men
			sprintf(buffer, "%s/men/%d.jpg", headlink.c_str(), index);
		}
		else { //women
			sprintf(buffer, "%s/women/%d.jpg", headlink.c_str(), index);
		}
		headurl = buffer;
	}
	else
	{
		headurl = ""; //is null, client use default.
	}
	uid = m_nStartID + rand() % m_nMaxCount;
	return true;
}

int Util::getRobotStartUid(int gameid, int levelid, int startid)
{
	int uid = 1 << ROBOT_FLAG_OFFSET;
	uid += gameid << GAME_ID_OFFSET;
	uid += levelid << LEVEL_ID_OFFSET;
	uid += startid << START_ID_OFFSET;
	
	LOGGER(E_LOG_INFO) << "gameid=" << gameid << ", levelid=" << levelid << ", startid=" << startid << ", startuid=" << uid;
 	return uid;
}

bool Util::fileExist(const char * filename)
{
	FILE * fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;
	}
}

void Util::taxDeduction(int64_t & get_coin, int tax, int & ded)
{
	if (get_coin == 0)
	{
		ded = 0;
		return;
	}

	if (tax >= 100 || tax <= 0)
	{
		tax = 5;
	}
	
	float ntax = float(tax) * 0.01;
	double gc = (double)get_coin;
	ded = int(gc * ntax + 0.5);
	get_coin = gc - ded;
}

std::vector<std::string> Util::split(const std::string & src, const std::string & tok, bool trim, std::string null_subst)
{
	std::vector<std::string> v;
	if (src.empty() || tok.empty())
		return v;
	std::string::size_type pre_index = 0, index = 0, len = 0;
	while ((index = src.find_first_of(tok, pre_index)) != std::string::npos)
	{
		if ((len = index - pre_index) != 0)
			v.push_back(src.substr(pre_index, len));
		else if (trim == false)
			v.push_back(null_subst);
		pre_index = index + 1;
	}
	std::string endstr = src.substr(pre_index);
	if (trim == false) v.push_back(endstr.empty() ? null_subst : endstr);
	else if (!endstr.empty()) v.push_back(endstr);
	return v;
}
/*
bool Util::getInvitePlayerTrumpt(std::string &strTrumpt, int nGame, int nLevel, int nTable, int nCurCount, int nMaxCount)
{
	const char *pGameDes = NULL;
	const char *pLevelDes = NULL;
	switch (nGame)
	{
		case E_SHOWHAND_GAME_ID:
			pGameDes = "梭哈";
			break;
		case E_BULLFIGHT_GAME_ID:
			pGameDes = "牛牛";
			break;
		case E_LANDLORD_GAME_ID:
			pGameDes = "斗地主";
			break;
		case E_FRUIT_GAME_ID:
			pGameDes = "水果机";
			break;
		case E_DRAGONTIGER_GAME_ID:
			pGameDes = "龙虎斗";
			break;
		case E_TEXAS_GAME_ID:
			pGameDes = "德州";
			break;
		case E_FLOWER_GAME_ID:
			pGameDes = "金花";
			break;
		case E_WENZHOU_MAHJONG_GAME_ID:
			pGameDes = "温州麻将";
			break;
		case E_HORSE_GAME_ID:
			pGameDes = "小马快跑";
			break;
		case E_FANFANLE_GAME_ID:
			pGameDes = "翻翻乐";
			break;
		case E_FISH_GAME_ID:
			pGameDes = "万炮捕鱼";
			break;
		case E_FRYBULL_GAME_ID:
			pGameDes = "炸金牛";
			break;
		default:
			LOGGER(E_LOG_WARNING) << "Unknown game id=" << nGame;
			return false;
	}

	switch (nLevel)
	{
		case E_PRIMARY_LEVEL:
			pLevelDes = "0.1元场";
			break;
		
		case E_MIDDLE_LEVEL:
			pLevelDes = "1元场";
			break;
		
		case E_ADVANCED_LEVEL:
			if (nGame == E_LANDLORD_GAME_ID)
			{
				pLevelDes = "3元场";
			}
			else
			{
				pLevelDes = "5元场";
			}
			break;

		case E_MASTER_LEVEL:
			pLevelDes = "10元场";
			break;

		case E_LEVEL_HUNDRED:
			pLevelDes = "百人场";
			break;
		default:
			LOGGER(E_LOG_WARNING) << "Unknown game id=" << nLevel;
			return false;
	}

	if (nCurCount < nMaxCount)
	{
		char acTrumpt[512] = { 0 };
		if ((E_LANDLORD_GAME_ID == nGame) || (E_WENZHOU_MAHJONG_GAME_ID == nGame))
			snprintf(acTrumpt, 512, "%s%s: %d缺%d, 速度来战!!!", pGameDes, pLevelDes, nCurCount, nMaxCount - nCurCount);
		else
			snprintf(acTrumpt, 512, "%s%s: %d缺1, 速度来战!!!", pGameDes, pLevelDes, nCurCount);
		strTrumpt = acTrumpt;
		return true;
	}
	else
	{
		return false;
	}
}

bool Util::getInvitePlayerTrumpt(std::string &strTrumpt, int nGame, int nAfterSecond)
{
	const char *pGameDes = NULL;
	switch (nGame)
	{
		case E_SHOWHAND_GAME_ID:
			pGameDes = "梭哈";
			break;
		case E_BULLFIGHT_GAME_ID:
			pGameDes = "牛牛";
			break;
		case E_LANDLORD_GAME_ID:
			pGameDes = "斗地主";
			break;
		case E_FRUIT_GAME_ID:
			pGameDes = "水果机";
			break;
		case E_DRAGONTIGER_GAME_ID:
			pGameDes = "龙虎斗";
			break;
		case E_TEXAS_GAME_ID:
			pGameDes = "德州";
			break;
		case E_FLOWER_GAME_ID:
			pGameDes = "金花";
			break;
		case E_WENZHOU_MAHJONG_GAME_ID:
			pGameDes = "麻将";
			break;
		case E_HORSE_GAME_ID:
			pGameDes = "小马快跑";
			break;
		case E_FANFANLE_GAME_ID:
			pGameDes = "翻翻乐";
			break;
		case E_FISH_GAME_ID:
			pGameDes = "万炮捕鱼";
			break;
		case E_FRYBULL_GAME_ID:
			pGameDes = "炸金牛";
			break;
		default:
			LOGGER(E_LOG_WARNING) << "Unknown game id=" << nGame;
			return false;
	}

	{
		char acTrumpt[512] = { 0 };
		snprintf(acTrumpt, 512, "决战百人%s之巅，速度来战!!!", pGameDes);
		strTrumpt = acTrumpt;
	}
	return true;
}
*/
	/*
redisContext * Util::initRedisContext(const char * ip, int port)
{
	struct timeval timeout = { 1, 0 };
	redisContext* redis = redisConnectWithTimeout(ip, port, timeout);
	
	if ((NULL == redis) || (redis->err))   //连接redis
	{
		LOGGER(E_LOG_INFO) << "connect redis failed, ip = " << ip << " port = " << port << " err = " << redis->errstr;
		if (redis)
		{
			redisFree(redis);
			redis = NULL;
		}
		return NULL;
	}

	if (REDIS_OK != redisSetTimeout(redis, timeout))
	{
		LOGGER(E_LOG_INFO) << "redis set timeout failed...";
		redisFree(redis);
		redis = NULL;
		return NULL;
	}
	return redis;
}*/

/*
int Util::parseJson(Json::Value& value, const char* key, int def)
{
    int v = def;
    try{
        v = value[key].asInt();
    }
    catch(...)
    {
        v = def;
    }
    return v;
}

std::string Util::parseJson(Json::Value& value, const char* key, std::string def)
{
    std::string  v = def;
    try{
        v = value[key].asString();
    }
    catch(...)
    {
        v = def;
    }
    return v;
}

bool Util::parseJsonBool(Json::Value& value, const char* key, bool def)
{
    bool  v = def;
    try{
        v = value[key].asBool();
    }
    catch(...)
    {
        v = def;
    }
    return v;
}


*/

std::string Util::getModulePath()
{
	char link_target[1024];
	char* last_slash;

	/* 读符号链接 /proc/self/exe 的目标。 */
	int ret = readlink ("/proc/self/exe", link_target, sizeof (link_target));
	if (ret == -1)
		/* readlink调用失败，所以中止。*/
		abort ();
	else
		/* 目标地址以 NUL 中止。 */
		link_target[ret] = '\0';
	/* 我们想对执行性文件名做去尾操作以获得包含它的目录。找到最右边的斜线。 */
	last_slash = strrchr (link_target, '/');
	if (last_slash == NULL || last_slash == link_target)
		/* 一些异常正在发生。 */
		abort ();
	*(last_slash+1) = '\0';

	return link_target;
}

std::string Util::UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
            else if (str[i] == ' ')
                strTemp += "+";
                else
                {
                    strTemp += '%';
                    strTemp += ToHex((unsigned char)str[i] >> 4);
                    strTemp += ToHex((unsigned char)str[i] % 16);
                }
    }
    return strTemp;
}






