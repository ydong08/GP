#include "CSetDefaultIcon.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "CConfig.h"
#include "RedisLogic.h"

int CSetDefaultIcon::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
   // Json::FastWriter jWriter;
    int iconid = Json::SafeConverToInt32(root["param"]["iconid"],1);
    int mid = Json::SafeConverToInt32(root["mid"]);
    out["status"] = 1;
    out["msg"] = "头像更新成功";

	std::string strKey = StrFormatA("UCNF%d",mid);
	RedisLogic::GetRdMinfo(mid)->HSET(strKey.c_str(),"iconid",StrFormatA("%d",iconid).c_str());

    out["iconid"] = iconid;
    out["icon"] = StrFormatA("%sdefault/%d.png", CConfig::getInstance().iconDomain.c_str(), iconid).c_str();
    //out = jWriter.write(ret);
    return status_ok;
}
