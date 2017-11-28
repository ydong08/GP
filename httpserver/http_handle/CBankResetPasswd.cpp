#include "CBankResetPasswd.h"
#include "json/json.h"
#include "Logger.h"
#include "StrFunc.h"
#include "md5.h"
#include "../threadres.h"
#include "JsonValueConvert.h"
#include "SqlLogic.h"

int CBankResetPasswd::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	//Json::Value jvRet;
	//Json::FastWriter jWriter;
	out["result"] = -1;
	if(root["mid"].isNull() || root["param"].isNull() || root["param"]["oldPWD"].isNull() || root["param"]["newPWD"].isNull()) 
	{
		out["msg"] = "请输入正确的参数";
		//out = jWriter.write(jvRet);
		return status_param_error;
	}

	int32_t nMid = Json::SafeConverToInt32(root["mid"]);
	std::string strOldBankPsw = root["param"]["oldPWD"].asString();
	std::string strNewBankPsw = root["param"]["newPWD"].asString();
	if (nMid < 0 || strNewBankPsw.empty() || strOldBankPsw.empty())
	{
		out["msg"] = "请输入正确的参数";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	if (strNewBankPsw == strOldBankPsw)
	{
		out["result"] = -2;
		out["msg"] = "原密码与新密码不能一样";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	CAutoUserDoing userDoing(nMid);
	if (userDoing.IsDoing())
	{
		out["result"] = -1;
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}

	if (!isPasswordInRule(strNewBankPsw) || !isPasswordInRule(strOldBankPsw))
	{
		out["result"] = -2;
		out["msg"] = "密码格式错误";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	MD5 md5NewPsw(strNewBankPsw);
	strNewBankPsw = md5NewPsw.md5();
	MD5 md5OldPsw(strOldBankPsw);
	strOldBankPsw = md5OldPsw.md5();

/*	MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
	if(NULL == dbMgr)
	{
		out["msg"] = "获取数据库信息出错";
		//out = jWriter.write(jvRet);
		return -1;
	}*/
	std::string strSqlBankPsw = SqlLogic::getBankPWD(nMid);
	if (strOldBankPsw != strSqlBankPsw)
	{
		out["result"] = -3;
		out["msg"] = "旧密码错误";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	std::map<std::string,std::string> mpInsert;
	mpInsert["bankPWD"] = strNewBankPsw;
	if(!SqlLogic::setUserInfo(nMid,mpInsert))
	{
		out["msg"] = "更新密码失败";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	out["result"] = 1;
	out["msg"] = "修改密码成功";
	//out = jWriter.write(jvRet);
    return status_ok;
}
