#include "CRegUserNameBinding.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Helper.h"
#include "TcpMsg.h"
#include "RedisLogic.h"
#include "SqlLogic.h"
#include "JsonValueConvert.h"
/**
 * 更新绑定状态（绑定成功）
 */
int updateBindingStatus(int sitemid, int mid, std::string strPhoneNo) 
{
    if (sitemid <= 0 || mid <= 0)  return false;

    MySqlDBAccess* DBMaster = ThreadResource::getInstance().getDBConnMgr()->DBMaster();
    string sql = StrFormatA("UPDATE ucenter.uc_account_binding SET status=1 WHERE sitemid=%d LIMIT 1", sitemid);
    int rows = DBMaster->ExeSQL(sql.c_str());
	if (rows > 0)
	{
		std::string strKey = StrFormatA("OTE|%d",mid);
		RedisLogic::GetRdOte(mid)->HSET(strKey.c_str(), "binding", "1");	//怎么更新的是这个redis ? 用户读取的时候又不走这里
		//更新用户信息 ,  php 登录的时候，直接走数据库了，不过还是更新一下redis吧，我们的C++代码还是走Redis的
		Json::Value info;
		info["binding"] = 1;
		info["phoneno_binding"] = strPhoneNo;
		RedisLogic::SetUserInfo(mid, info, 2 * 24 * 3600); //写入redis
	}

    return rows;
}




int CRegUserNameBinding::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
    Json::FastWriter jWriter;
    int mid     = Json::SafeConverToInt32(root["mid"]);
    int sitemid = Json::SafeConverToInt32(root["param"]["sitemid"]);
    string phoneno = root["param"]["phoneno"].asString();
	phoneno = Helper::filterInput(phoneno);

    int cicode = Json::SafeConverToInt32(root["param"]["idcode"]);
    out["result"] = 0;
	if (mid < 1 || sitemid < 1 || cicode < 10000 || phoneno.empty())
	{
		out["result"] = -1;
		out["msg"] = "参数不正确";
		//out = jWriter.write(ret);
		return status_ok;
	}

	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["result"] = -1;
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}

    if(!isMobile(phoneno)){
        out["result"] = -1;
        out["msg"] = "手机号格式错误";
        //out = jWriter.write(ret);
        return status_ok;
    }

    int server_idcode = RedisLogic::codeCheck(phoneno);
    if(server_idcode == -1 || server_idcode != cicode){
        out["result"] = -2;
        out["msg"] = "验证码错误";
        //out = jWriter.write(ret);
        return status_ok;
    }

	int nRet = updateBindingStatus(sitemid, mid, phoneno);
    out["result"] = nRet;
	if (nRet > 0)
	{
		Json::Value userInfo;
		if(SqlLogic::getOneById(userInfo, mid))
			RedisLogic::addWin(userInfo, 300, MONEY_REGISTE, 0, "账号绑定金币变化(C++)");

        out["phoneno_binding"] = phoneno;
        out["msg"] = "绑定成功";
	}
    else
    {
        out["result"] = -1;
        out["msg"] = "您已绑定过，如有疑问，请联系客服.";
    }

    //out = jWriter.write(ret);
    return status_ok;
}
