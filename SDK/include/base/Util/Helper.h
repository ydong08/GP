//
// Created by new on 2016/11/8.
//

#ifndef HTTPSVR_HELPER_H
#define HTTPSVR_HELPER_H

#include <string>
#include <vector>
#include "json/json.h"

using namespace std;

class Helper {
public:
    /**
     * 过滤用户输入字符中的不合法字符
     * 目前主要直接把一些特殊字符替换成空格，并去掉前后空格
     * @param    string        $str    用户输入的字符
     * @return    string        过滤后输出的字符
     */
    static string filterInput(string str);

    static int strtotime(string datestr);

    static vector<string> explode(const char * str, const char * sep);

    static string implode(const char* sep, const vector<string>& strarr);
    static string implode(const char* sep, const std::vector<int>& vec);

    static string strtolower(const char* str);

    static string trim(const char *str);

    static bool isUsername(const char *str);

    static long ip2long(const char* ip);
    static std::string long2ip(long ip);

    static string replace(const string& str, const string& src, const string& dest);

	static int time2morning();

	void unsetUserFiled(Json::Value& jvUser);
};

#endif //HTTPSVR_HELPER_H
