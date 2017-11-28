#ifndef _TYPE_DEF_H
#define _TYPE_DEF_H

#include <string>
#include <vector>
#include <cassert>
#include "Typedef.h"

#define ROBOT_FLAG_OFFSET	(30)
#define GAME_ID_OFFSET		(22)
#define LEVEL_ID_OFFSET		(17)
#define START_ID_OFFSET		(12)
#define MAX_ROBOT_NUMBER	(2048)

#define IS_ROBOT_UID(uid)	(((uid >> ROBOT_FLAG_OFFSET) && 0x01) == 1)


#ifndef WIN32
#define _atoi64(val) strtoll(val, NULL, 10)
#endif
#define atoi64(val) _atoi64(val)

class Util : public DisableCopy
{
public:
	Util() {}
	~Util() {}
	
	static void registerSignal();

	static void daemonize();
	
	static int getGMTTime_Y();
	static long getGMTTime_S();
	static long getGMTTime_U();

	static std::string formatGMTTime();
	
    static std::string formatGMTTimeS();
    
	static std::string formatGMTTime_Date();
	
	static int getTimeDifferenceS();
    static bool isSameDay(const time_t time1 , const time_t time2);
    static bool isToday(const time_t time1);

	static bool getLocalIp(std::string &str);

	static bool mkdir(const std::string& path);

	static bool dir_exist(const std::string& name);

	template <typename _Tp, typename _CharT>
	static const _Tp& split_path(std::basic_string<_CharT> path, _CharT delim, _Tp& entries);
    
    static unsigned char ToHex(unsigned char x)
    {
        return  x > 9 ? x + 55 : x + 48;
    }
    
    static std::string UrlEncode(const std::string& str);

	/*
	 headlink : input
	 name  : output
	 sex      : output
	 headurl  : output
	*/
	static bool makeRobotNameSex(std::string headlink, std::string &name, int &sex, std::string &headurl);
	static bool makeRobotInfo(std::string headlink, int &sex, std::string &headurl, int &uid);

	static int getRobotStartUid(int gameid, int levelid, int startid);

	static bool fileExist(const char *filename);

	static void taxDeduction(int64_t& get_coin, int tax, int& ded);

	
	static std::vector<std::string> split(const std::string& src, const std::string& tok, bool trim, std::string null_subst);


	static inline int rand_range(int _min, int _max)
	{
		assert(_min < _max);
		return _min + rand() % (_max - _min);
	}

	static inline float rand_range(float _min, float _max)
	{
		assert(_min < _max);
		return _min + (_max - _min) * (float)rand() / (float)RAND_MAX;
	}

	/*
	 * 获取当前执行文件路径
	 */
	static std::string getModulePath();
private:
	static const int m_nStartID  = 4000000;
	static const int m_nMaxCount = 999999;
};

#endif
