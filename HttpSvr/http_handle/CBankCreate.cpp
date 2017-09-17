#include "CBankCreate.h"
#include "json/json.h"
#include "Logger.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <regex>

#include "../threadres.h"
#include "MySqlDBAccess.h"
#include "md5.h"
#include "JsonValueConvert.h"
#include "SqlLogic.h"

using namespace std;

int CBankCreate::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	//Json::Value jRet;

	out["result"] = 0;
	out["msg"] = "密码格式错误";
	if (root["mid"].isNull() || root["param"]["bankPWD"].isNull())
	{
		////out = jWriter.write(jRet);
		return status_ok;
	}
	int32_t nMid  = Json::SafeConverToInt32(root["mid"]);	//保险箱id
	if (nMid <= 0)
	{
		out["msg"] = "nMid不正确";
		return status_ok;
	}
	CAutoUserDoing userDoing(nMid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}
	std::string strBankPWD = root["param"]["bankPWD"].asString();

	if (!isPasswordInRule(strBankPWD))
	{
		////out = jWriter.write(jRet);
        out["msg"] = "密码不符合规则";
		return status_ok;
	}

	/*MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
	if(NULL == dbMgr)
	{
		////out = jWriter.write(jRet);
		return -2;
	}*/
	std::string strSqlBankPsw = SqlLogic::getBankPWD(nMid);
	if (!strSqlBankPsw.empty())
	{
		out["msg"] = "已设置过保险箱密码";
		////out = jWriter.write(jRet);
		return status_ok;
	}

	MD5 md5(strBankPWD);
	strBankPWD = md5.md5();
	std::map<std::string,std::string> mpInsert;
	mpInsert["bankPWD"] = strBankPWD;
	if(!SqlLogic::setUserInfo(nMid,mpInsert))
	{
		out["msg"] = "写入密码失败";
		////out = jWriter.write(jRet);
		return status_ok;
	}

	out["result"] = 1;
	out["msg"] = "设置保险箱密码成功";
	////out = jWriter.write(jRet);
    return status_ok;
}
