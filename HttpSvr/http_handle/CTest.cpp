//
//  CTest.cpp
//  HttpSvr
//
//  Created by yuzhan on 2016/11/15.
//
//

#include "CTest.h"
#include "Logger.h"
#include "JsonValueConvert.h"
#include "RedisAccess.h"
#include "threadres.h"
#include "RedisLogic.h"
#include "SqlLogic.h"
#include "json/json.h"
#include "Lib_Ip.h"
#include "Helper.h"
#include "Util.h"

enum TestType
{
    Test_MySQL = 0,
    Test_Redis = 1,
};

string& __trim(string &str, string::size_type pos = 0)
{
    static const string delim = " \t"; //删除空格或者tab字符
    pos = str.find_first_of(delim, pos);
    if (pos == string::npos)
        return str;
    return __trim(str.erase(pos, 1));
}

void test_hmset(const Json::Value& value, std::string& out)
{
 /*   auto redis = ThreadResource::getInstance().getRedisConnMgr()->ucenter();
    std::string hash = value["hash"].asString();
    std::string key1 = value["key1"].asString();
    std::string value1 = value["value1"].asString();
    std::vector<std::string> key; key.push_back(key1);
    std::vector<std::string> val; val.push_back(value1);
    redis->hmset2(hash, key, val);*/
}

void test_redis(const Json::Value& value, std::string& out)
{
    auto redis = ThreadResource::getInstance().getRedisConnMgr()->ote(6878007);
    std::map<std::string, std::string> ret;
    redis->HGetAll("OTE|6878007", ret);
    LOGGER(E_LOG_INFO) << ret;
}

int CTest::do_request(const Json::Value &root, char *client_ip, HttpResult &out)
{
	int start = Helper::strtotime(Util::formatGMTTime_Date() + " 00:00:00");
	int end = Helper::strtotime(Util::formatGMTTime_Date() + " 23:59:59");

	return status_param_error;
	Json::Value info;
	SqlLogic::GetUserInfo(6877680, info);

	vector<string> ip_arr = Lib_Ip::find(client_ip);
	if (ip_arr[0] == "中国")
	{
		int iii = 0;
	}

/*	Json::Reader jReader;
	Json::Value jvTest;
	std::string strJson = "{\"otherpay\":\"6\",\"phoneRegister\":0,\"sceneSwitch\":0,\"bank\":0}";
	if (jReader.parse(strJson,jvTest))
	{
		int otherpay = Json::SafeConverToInt32(jvTest["otherpay"]);
		int phoneRegister = Json::SafeConverToInt32(jvTest["phoneRegister"]);
	}

	*/
	RedisLogic::test();
    LOGGER(E_LOG_INFO) << root.toStyledString();
    int type = Json::SafeConverToInt32(root["testtype"]);
    int interface = Json::SafeConverToInt32(root["testinterface"]);
    Json::FastWriter writer;
    if (type == Test_MySQL)
    {
        
    }
    else if (type == Test_Redis)
    {
        if (interface == 0)
        {
            std::string res;
            test_hmset(root["param"], res);
            //Json::Value info;
            out["msg"] = res;
            //out = writer.write(info);
        }
        else if (interface == 1)
        {
            std::string out;
            test_redis(root, out);
        }
    }
    else
    {
        
    }
    return 0;
}
