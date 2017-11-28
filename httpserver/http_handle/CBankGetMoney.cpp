#include "CBankGetMoney.h"
#include "json/json.h"
#include "Logger.h"
#include <vector>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include "../threadres.h"
#include "StrFunc.h"
#include "md5.h"
#include "JsonValueConvert.h"
#include "SqlLogic.h"
#include "RedisLogic.h"

int CBankGetMoney::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	//Json::Value jvRet;
	//Json::FastWriter jWriter;
	out["result"] = -1;
	if(root["mid"].isNull() || root["param"].isNull() || root["param"]["money"].isNull() || root["param"]["bankPWD"].isNull() || root["gameid"].isNull()) 
	{
		out["msg"] = "请输入正确的参数";
		//out = jWriter.write(jvRet);
		return status_ok;
	}
	int32_t nMid = Json::SafeConverToInt32(root["mid"]);
	int64_t nMoney = Json::SafeConverToInt64(root["param"]["money"]);
	std::string strBankPwd = root["param"]["bankPWD"].asString();
	int32_t nGameId = Json::SafeConverToInt32(root["gameid"]);
	int32_t nProductId = Json::SafeConverToInt32(root["productid"],1);
	if (nMid < 0)
	{
		out["msg"] = "请输入正确的用户ID";
		return status_ok;
	}
	CAutoUserDoing userDoing(nMid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}
	if (nMoney < 1)
	{
		out["msg"] = "请输入正确的金额";
		return status_ok;
	}
	if (!isPasswordInRule(strBankPwd))
	{
		out["result"] = -2;
		out["msg"] = "密码格式错误";
		return status_ok;
	}

	//判断其它线程是不是正在读写，防止快速存取款
	if (RedisLogic::IsFastCtrlMoney(nMid))
	{
		out["msg"] = "您操作太快了，请喝杯茶休息一会";
		return status_user_error;
	}

	RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
	if (redisMgr == NULL) 
	{
		out["msg"] = "连接reids出错";
		return status_user_error;
	}

	RedisAccess* pUserRedisSvr = redisMgr->userServer(nMid);
	if (pUserRedisSvr == NULL) 
	{
		out["msg"] = "连接reids出错";
		return status_user_error;
	}
	vector<string> vsFields;
	vsFields.push_back("svid");
	vsFields.push_back("tid");
	vsFields.push_back("level");
	vsFields.push_back("gameid");
	map<string,string> mpRet;
	string strKey = StrFormatA("User:%" PRIi32 ,nMid);

	if (pUserRedisSvr->HMGET(strKey.c_str(),vsFields,mpRet))
	{
//		out["msg"] = "获取用户缓存信息出错";
//		return status_ok;
        if (Json::SafeConverToInt32(mpRet["svid"]) > 0 && Json::SafeConverToInt32(mpRet["tid"]) > 0 && Json::SafeConverToInt32(mpRet["level"]) > 0)
        {
            out["msg"] = "游戏中，不能操作保险箱";
            out["result"] = -3;
            return status_ok;//下注状态，不能存
        }
	}

	MD5 md5(strBankPwd);
	strBankPwd = md5.md5();

	std::string strSqlBankPsd = SqlLogic::getBankPWD(nMid);
	if (strBankPwd != strSqlBankPsd)
	{
		out["msg"] = "密码错误";
		return status_ok;
	}
	Json::Value jvRetGameInfo;
	if(!SqlLogic::GetGameInfo(nMid, jvRetGameInfo))
	{
		out["msg"] = "获取数据库信息出错";
		return status_ok;
	}
	int64_t nFreeMoney = Json::SafeConverToInt64(jvRetGameInfo["freezemoney"]);
	if (nFreeMoney < nMoney)
	{
		out["result"] = -4;
		out["msg"] = "保险箱金币少于要取出的金币";
		return status_ok;
	}

	Json::Value jvMoneyRet;
	if(!SqlLogic::getMoneyFromBank(jvMoneyRet, nMid, nMoney, nGameId, nProductId))
	{
		out["msg"] = "保险箱操作失败";
		return status_ok;
	}
	out["result"]      = 1;
	out["freezemoney"] = Json::SafeConverToInt64(jvMoneyRet["freezemoney"]);
	out["money"]       = Json::SafeConverToInt64(jvMoneyRet["money"]);
	out["msg"] = "取钱成功";
    return status_ok;
}
