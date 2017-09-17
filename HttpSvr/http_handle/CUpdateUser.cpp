#include "CUpdateUser.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "JsonValueConvert.h"
#include "SqlLogic.h"
#include "RedisLogic.h"

int CUpdateUser::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value 	 ret;
    Json::FastWriter write;
    out["result"] = 0;

	if (root["param"]["sex"].isNull()) //root["param"]["mnick"].isNull() ||
	{
		out["msg"] = "性别为空";
		//out = write.write(ret);
		return status_param_error;
	}
    
    int mid = Json::SafeConverToInt32(root["mid"]);
	int sid = Json::SafeConverToInt32(root["sid"]);
	//int sex = Json::SafeConverToInt32(root["param"]["sex"]);
    if (mid < 1)
    {
		out["msg"] = "mid不正确";
		return status_param_error;
    }
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}

    std::string sex = TrimStr(root["param"]["sex"].asString());
	if(sex.empty()) // || mnick.empty()
	{
		out["msg"] = "性别为空";
		return status_ok;
	}
	std::string mnick = TrimStr(root["param"]["mnick"].asString());
	if (!mnick.empty())
	{
		if (!isNickNameInRule(mnick))
		{
			out["msg"] = "昵称不符合要求，请更改";
			return status_ok;
		}
		RedisAccess* rdAccount = RedisLogic::GetRdAccount();
		if (NULL == rdAccount)
		{
			LOGGER(E_LOG_ERROR) << "redis account is NULL!";
			out["msg"] = "缓存服务器出错";
			return status_ok;
		}

		Json::Value user;
		if (!SqlLogic::getOneById(user, mid)) 
		{
			out["msg"] = "找不到用户";
			return status_ok;
		}
		bool bSAdd = rdAccount->SADD("USMNICK",mnick.c_str());
		if (mnick != user["mnick"].asString() && !bSAdd)
		{
			out["msg"] = "昵称已存在";
			out["result"] = -1;
			return status_ok;
		}
	}
    std::string hometown = TrimStr(root["param"]["hometown"].asString());
    
    std::map<std::string,std::string> info;
    if(!mnick.empty()) info["mnick"]    = mnick;
    info["sex"]      = sex;
    if(!hometown.empty()) info["hometown"] = hometown;
    bool flag = SqlLogic::setUserInfo(mid, info);
    out["result"] = flag ? 1 : 0;
    out["msg"]    = "更新成功";
    //out = write.write(ret);
    return status_ok;
}
























