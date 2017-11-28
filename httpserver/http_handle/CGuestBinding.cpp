#include "CGuestBinding.h"
#include "json/json.h"
#include "Logger.h"
#include "StrFunc.h"
#include "../threadres.h"
#include "RedisAccess.h"
#include "Helper.h"
#include <algorithm>
#include "md5.h"
#include "threadres.h"
#include "RedisLogic.h"
#include "SqlLogic.h"

namespace
{
    static std::vector<int> errorCode = {-1,-2,-3,-4,-5};
    static std::vector<std::string> error_string = {
        "参数错误",
        "手机号，用户名或密码为空",
        "用户名不合法",
        "用户名已存在或该手机已经绑定过",
        "验证码错误"
    };
    
    std::string getErrorMsg(int ec)
    {
        auto it = std::find(errorCode.begin(), errorCode.end(), ec);
        size_t index = std::distance(errorCode.begin(), it);
        if (index >= error_string.size())
        {
            return "";
        }
        return error_string[index];
    }
    
    int errorMsg(int code, Json::Value& info, Json::FastWriter& writer, HttpResult& out)
    {
        out["msg"] = getErrorMsg(code);
        out["result"] = code;
       // out = writer.write(info);
        return code;
    }
}

int CGuestBinding::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    Json::FastWriter jWriter;
    //Json::Value ret;
    out["result"] = 0;
    int nMid, sid, type, idcode;	//nSitemId, 
//	std::string strSiteMid;
    std::string username, phoneno, password;
    try {
        nMid = root["mid"].asInt();
        sid = root["sid"].asInt();
    //    nSitemId = Json::SafeConverToInt32(root["param"]["sitemid"]);
	//	strSiteMid = root["param"]["sitemid"].asString();
        out["type"] = type = root["param"]["type"].asInt();
        idcode = root["param"]["idcode"].asInt();
        out["phoneno"] = phoneno = root["param"]["phoneno"].asString();
        out["username"] = username = Helper::filterInput(Helper::strtolower(root["param"]["username"].asString().c_str()));
        out["password"] = password = Helper::trim(root["param"]["password"].asString().c_str());
    } catch (...) {
        out["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_ok;
    }

    if (!isMobile(phoneno))
        phoneno = "";

    out["rewardMoney"] = 0;
    out["money"] = 0;
    
    if (nMid < 1 || (sid != 100 && sid != 102)) //nSitemId < 1 || 
    {
        errorMsg(-1, out, jWriter, out);
		return status_ok;
    }

	CAutoUserDoing userDoing(nMid);
	if (userDoing.IsDoing())
	{
		errorMsg(-1, out, jWriter, out);
		return status_ok;
	}
    
    int rewardMoney = 0;
    bool flag;
    if (type == 1) //游客绑定账号
    {
        //先判断是否手机号
        if (isMobile(username))
        {
            errorMsg(-3, out, jWriter, out);
			return status_ok;
        }
        if (username.empty() || password.empty())
        {
			errorMsg(-2, out, jWriter, out);
			return status_ok;
        }
        if (!Helper::isUsername(username.c_str()))
        {
			errorMsg(-3, out, jWriter, out);
			return status_ok;
        }
        if (!ThreadResource::getInstance().getRedisConnMgr()->account()->SADD("USN", username.c_str()))
        {
			errorMsg(-4, out, jWriter, out);
			return status_ok;
        }
        flag = SqlLogic::guestBindAccount(username, password, nMid);
    }
    else if (type == 2) //游客绑定手机号
    {
        if (phoneno.empty() || password.empty())
        {
            errorMsg(-2, out, jWriter, out);
			return status_ok;
        }
        int svr_idcode = RedisLogic::getServerIdCode(phoneno);
        if (svr_idcode != idcode)
        {
            errorMsg(-5, out, jWriter, out);
			return status_ok;
        }
        if (!ThreadResource::getInstance().getRedisConnMgr()->account()->SADD("PHN", phoneno.c_str()))
        {
            errorMsg(-4, out, jWriter, out);
			return status_ok;
        }
        Json::Value data;
        data["mid"] = nMid;
        data["phoneno"] = phoneno;
        data["password"] = password;
        flag = SqlLogic::registerByPhoneNumber(data, 2);
    }
    else
    {
        LOGGER(E_LOG_ERROR) << "unknown type = " << type;
        errorMsg(-1, out, jWriter, out);
		return status_ok;
    }
    if (flag)
    {
        Json::Value gameinfo;
        if (!SqlLogic::getOneById(gameinfo, nMid))
        {
            errorMsg(-1, out, jWriter, out);
			return status_ok;
        }
        out["rewardMoney"] = rewardMoney;
        out["money"] = gameinfo["money"];
        out["sitemid"] = gameinfo["sitemid"];
        out["result"] = 1;
    }
    out["msg"] = "绑定成功";
    return status_ok;
}
