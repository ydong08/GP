#include "CRegResetPassword.h"
#include "json/json.h"
#include "Logger.h"
#include <string>

#include "MySqlDBAccess.h"
#include "../threadres.h"
#include "JsonValueConvert.h"
#include "SqlLogic.h"


int CRegResetPassword::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	//Json::Value jvRet;
	Json::FastWriter jWriter;
	out["result"] = -1;
	if(root["sid"].isNull() || root["mid"].isNull() || root["param"].isNull() || root["param"]["oldpassword"].isNull() || root["param"]["newpassword"].isNull()) 
	{
		out["msg"] = "请输入正确的参数";
		//out = jWriter.write(jvRet);
		return status_argment_access;
	}
	int32_t nSId = Json::SafeConverToInt32(root["sid"]); 
	int32_t nSiteMid = Json::SafeConverToInt32(root["param"]["sitemid"]);
	std::string strOldBankPsw = root["param"]["oldpassword"].asString();
	std::string strNewBankPsw = root["param"]["newpassword"].asString();
	int32_t nMid = Json::SafeConverToInt32(root["mid"]);
	if (nMid < 1)
	{
		out["msg"] = "请输入正确的参数";
		return status_argment_access;
	}
	CAutoUserDoing userDoing(nMid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}

	Json::Value info;
	if (!SqlLogic::GetUserInfo(nMid, info)) //通过MySql获取用户信息
	{
		out["msg"] = "获取用户信息出错";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	if(info["sitemid"].isNull())
	{
		out["msg"] = "获取用户信息失败";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	nSiteMid = Json::SafeConverToInt32(info["sitemid"]);//解决游戏绑定普通账号后不能马上修改密码的情况 
	
	out["result"] =  SqlLogic::resetPassword(nSiteMid, nSId, strOldBankPsw, strNewBankPsw);
	out["msg"] = "修改密码成功";
	//out = jWriter.write(jvRet);
	return status_ok;
}


