#ifndef _TYPE_DEF_H
#define _TYPE_DEF_H

#include <string>
#include <vector>
#include <cassert>
#include "Typedef.h"
#include "Util_struct.h"

#ifndef WIN32
#define _atoi64(val) strtoll(val, NULL, 10)
#endif
#define atoi64(val) _atoi64(val)

namespace Util
{
    /**
     * 注册信号量
     */
    void registerSignal();
    /**
     * 后台化进程
     */
	void daemonize();
    /**
     * 获取当前日期秒数
     * @return
     */
	long getGMTTime_S();
    /**
     * 获取时间字符串，精确到微秒
    * @return 返回"2016-11-02 12:12:22:12300001"格式的字符串
    */
	std::string formatGMTTime();
	/**
	 * 获取时间字符串，精确到秒
	 * @return 返回"2016-11-02 12:12:22"格式的字符串
	 */
    std::string formatGMTTimeS();
    /**
     * 获取日期字符串
     * @return 返回"2016-11-02"格式的字符串
     */
	std::string formatGMTTime_Date();
	/**
	 * 获取当前是多少号
	 * @return
	 */
	int getTimeDay();
	
	int getTimeDifferenceS();
    /**
     * 获取本地IP
     * @param str 返回的ip字符串
     * @return
     */
	bool getLocalIp(std::string &str);
    /**
     * 创建路径
     * @param path
     * @return
     */
	bool mkdir(const std::string& path);
    /**
     * 判断路径是否存在
     * @param name 路径名
     * @return
     */
	bool dir_exist(const std::string& name);
	/**
	 * 判断文件是否存在
	 * @param name 路径名
	 * @return
	 */
	bool file_exist(const std::string& name);

	template <typename _Tp, typename _CharT>
	const _Tp& split_path(std::basic_string<_CharT> path, _CharT delim, _Tp& entries)
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
    /**
     * 分割字段串为数组
     * @param str 原字符串
     * @param sep 分割符
     * @return 字符串数组
     */
    std::vector<std::string> explode(const char * str, const char * sep);
    /**
     * 获取当前执行文件路径
     * @return
     */
    std::string getModulePath();
    /**
     * 解析地址字符串
     * @param szAddress 地址字符串，格式为DOMAIN-NAME:PORT
     * @param address 返回解析后的Address
     * @return
     */
     bool parseAddress(const char* szAddr, AddressInfo &address);
	 /// <summary>
	 /// 获取指定范围随机值，[_min, _max)
	 /// </summary>
	 /// <param name="_min"></param>
	 /// <param name="_max"></param>
	 /// <returns></returns>
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
};

#endif
