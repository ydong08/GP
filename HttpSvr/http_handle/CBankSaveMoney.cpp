#include "CBankSaveMoney.h"
#include "json/json.h"
#include "Logger.h"
#include "RedisAccess.h"
#include "../threadres.h"
#include "Util.h"
#include "RedisLogic.h"

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <string>
#include <vector>
#include <map>

#include <inttypes.h>
#include "JsonValueConvert.h"
#include "SqlLogic.h"

using namespace std;
//为什么没验证密码？
int CBankSaveMoney::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	//Json::Value jRet;
	//Json::FastWriter jWriter;

	out["result"] = 0;
	if (root["mid"].isNull() || root["param"]["money"].isNull() || root["gameid"].isNull())
	{
		out["msg"] = "请输入正确的金额";
		//out = jWriter.write(jRet);
		return status_ok;
	}
	
	int32_t nMid = Json::SafeConverToInt32(root["mid"]);
	int64_t uMoney = Json::SafeConverToInt64(root["param"]["money"]);
	int32_t nGameId = Json::SafeConverToInt32(root["gameid"]);
	int32_t uProductId = Json::SafeConverToInt32(root["productid"],1);

	//判断其它线程是不是正在读写，防止快速存取款
/*	if (RedisLogic::IsFastCtrlMoney(nMid))
	{
		out["msg"] = "您操作太快了，请喝杯茶休息一会";
		return status_user_error;
	}*/
	CAutoUserDoing userDoing(nMid);
	if (userDoing.IsDoing())
	{
		out["result"] = -1;
		out["msg"] = "您操作太快了，请休息一会！";
		return status_user_error;
	}

	RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
	if (redisMgr == NULL) 
	{
		out["msg"] = "连接reids出错";
		//out = jWriter.write(jRet);
		return status_user_error;
	}

	RedisAccess* pUserRedisSvr = redisMgr->userServer(nMid);
	if (pUserRedisSvr == NULL) 
	{
		out["msg"] = "连接reids出错";
		//out = jWriter.write(jRet);
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
		if (atoi(mpRet["svid"].c_str()) > 0 && atoi(mpRet["tid"].c_str()) > 0 && atoi(mpRet["level"].c_str()) > 0)
		{
			out["msg"] = "游戏中，不能操作保险箱";
			out["result"] = -3;
			//out = jWriter.write(jRet);
			return status_ok;//下注状态，不能存
		}
	/*	out["msg"] = "获取reids信息出错";
		//out = jWriter.write(jRet);
		return -1;*/
	}

	Json::Value info;
	//读出来的全部是String类型的
	//if (!RedisLogic::GetUserInfo(nMid, info)) return false;
	if (!SqlLogic::GetGameInfo(nMid, info)) return false;
	if(info["money"].isNull() || Json::SafeConverToInt64((info["money"])) < uMoney)
	{
		out["result"] = -1;
		out["msg"] = "身上金币小于要存的金币";
		//out = jWriter.write(jRet);
		return status_ok;
	}

/*	MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
	if(NULL == dbMgr)
	{
		out["result"] = -1;
		out["msg"] = "获取服务信息出错";
		//out = jWriter.write(jRet);
		return -1;
	}*/

	Json::Value jvMoneyRet;
	if(!SqlLogic::saveMoney2Bank(jvMoneyRet, nMid, uMoney, nGameId, uProductId))
	{
		out["msg"] = "存钱失败";
		//out = jWriter.write(jRet);
		return status_ok;
	}

	out["result"]      = 1;
	out["freezemoney"] = Json::SafeConverToInt64(jvMoneyRet["freezemoney"]);
	out["money"]       = Json::SafeConverToInt64(jvMoneyRet["money"]);
	out["msg"] = "存入保险箱成功";
	//out = jWriter.write(jRet);
    return status_ok;
}
