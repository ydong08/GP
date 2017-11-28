#include "CRegGetPassword.h"
#include "json/json.h"
#include "Logger.h"
#include <string>

//#include "MySqlDBAccess.h"
//#include "../threadres.h"
#include "JsonValueConvert.h"
#include "SqlLogic.h"
#include "Helper.h"
#include "HttpLib.h"


int CRegGetPassword::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	out["result"] = 0;
	if (root["mid"].isNull() || root["productid"].isNull())
	{
		out["msg"] = "参数不正确";
		return status_argment_access;
	}
	std::string strPhoneNo = root["param"]["phoneno"].asString();
	if (strPhoneNo.empty()) 
	{
		out["msg"] = "手机号码不能为空";
		return status_ok;
	}

	strPhoneNo = Helper::filterInput(strPhoneNo);
	
	int32_t nMid = Json::SafeConverToInt32(root["mid"]);
	int32_t nProductId = Json::SafeConverToInt32(root["productid"]);
	if (nProductId < 0)
	{
		out["msg"] = "产品ID不正确";
		return status_ok;
	}
	if (!isMobile(strPhoneNo))
	{
		out["result"] = -1;
		out["msg"] = "手机号错误";
		return status_ok;
	}
	int64_t nCount = SqlLogic::limitCount(strPhoneNo, "101", 1, true, Helper::time2morning());
	if (nCount > 5)
	{
		out["result"] = -2;
		out["msg"] = "获取次数过多";
		return status_ok;
	}
	Json::Value jvMsgInfo;
	if (!SqlLogic::getPasswordByphoneNo(jvMsgInfo,strPhoneNo,nProductId))
	{
		out["result"] = -3;
		out["msg"] = "获取密码失败";
		return status_ok;
	}
	bool result = HttpLib::sendMessage(jvMsgInfo, strPhoneNo, 2);

	if (!result)
	{
		out["msg"] = "发送密码短信失败";
		return status_ok;
	}

	out["result"] = 1;
	out["msg"] = "获取密码成功";
	return status_ok;
}


