//
//  CReloadConfig.cpp
//  HttpSvr
//
//  Created by yuzhan on 2016/11/19.
//
//

#include "CReloadConfig.hpp"

#include "Logger.h"
#include "CConfig.h"

/*
{
    "user": "",
    "param":
    {
        "method":"admin.reloadconfig"
    }
}
*/

int CReloadConfig::do_request(const Json::Value &root, char *client_ip, HttpResult &out)
{
    if (CConfig::getInstance().Reload())
    {
        out["msg"] = "重新加载配置成功";
        return status_ok;
    }
    else
    {
        out["msg"] = "重新加载配置失败";
        return status_ok;
    }
    return status_ok;
}
