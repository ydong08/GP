//
//  CCheckIdCode.cpp
//  HttpSvr
//
//  Created by yuzhan on 2016/11/10.
//
//

#include "CCheckIdCode.h"
#include "json/json.h"
#include "Logger.h"
#include "RedisAccess.h"
#include "../threadres.h"
#include "Helper.h"
#include "JsonValueConvert.h"
#include "RedisLogic.h"


int CCheckIdCode::do_request(const Json::Value &root, char *client_ip, HttpResult &out)
{
    //Json::Value info;
    out["result"] = 0;
    std::string phoneno;
    int idcode, type;
    try {
        phoneno = Helper::filterInput(root["param"]["phoneno"].asString());
        idcode = Json::SafeConverToInt32(root["param"]["idcode"]);
        type = Json::SafeConverToInt32(root["param"]["type"]);
	} catch (...) {
        out["msg"] = "缺少参数";
        //out = writer.write(info);
        return status_param_error;
    }
    if (phoneno.empty() || idcode < 10000 || type < 0) 
	{
        out["msg"] = "参数不正确";
        //out = writer.write(info);
        return status_ok;
    }

  /*  RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
	if (NULL == redisMgr)
	{
		out["msg"] = "获取Redis信息出错";
		//out = writer.write(info);
		return -1;
	}*/
    int server_idcode = RedisLogic::getServerIdCode(phoneno);
    if (server_idcode != idcode)		//server_idcode == -1
	{
        out["msg"] = "验证码错误";
        //out = writer.write(info);
        return status_vercode_error;
    }
    out["type"] = type;
    out["result"] = 1;
    //out = writer.write(info);
    return status_ok;
}
