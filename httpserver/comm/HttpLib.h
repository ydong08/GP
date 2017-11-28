//
// Created by new on 2016/11/11.
//

#ifndef HTTPSVR_HTTPLIB_H
#define HTTPSVR_HTTPLIB_H

#include <string>
#include "evhttp.h"
#include "json/json.h"

using namespace std;

class HttpLib {
public:
    static string encode_xor(string str);
    static int GET(const char* url);
    static int POST(const char *url, const char* postdata);
    /*
     * 发送短信验证码
     */
    static bool sendMessage(Json::Value content, string phoneno, int type, int mid=0);
};


#endif //HTTPSVR_HTTPLIB_H
