#include "CBankFindPasswd.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "JsonValueConvert.h"
#include "RedisLogic.h"
#include "SqlLogic.h"

int CBankFindPasswd::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    out["result"] = 0;

	if (root["mid"].isNull() || root["param"]["phoneno"].isNull() || root["param"]["idcode"].isNull())
	{
		out["msg"] = "参数不正确";
		return status_ok;
	}
    
    int mid = Json::SafeConverToInt32(root["mid"]);
	if (mid < 1)
	{
		out["msg"] = "参数不正确";
		return status_ok;
	}
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}
    std::string phoneno = root["param"]["phoneno"].asString();
    int idcode  = Json::SafeConverToInt32(root["param"]["idcode"]);
	if (idcode < 10000)
	{
		out["msg"] = "参数不正确";
		//out = jsonWriter.write(jsonRet);
		return status_ok;
	}
    
    int serverIdCode = getCacheIdCode(phoneno);
    if (idcode != serverIdCode) 
	{
        out["msg"] = "验证码错误";
        //out = jsonWriter.write(jsonRet);
        return status_ok;
    }
    
    if(!setUserBackPassword(mid))
	{
		out["msg"] = "重置密码失败";
		//out = jsonWriter.write(jsonRet);
		return status_ok;
	}
    out["result"] = 1;
    out["bank"] = 0;
    out["msg"]  = "保险箱密码重置成功,请尽快设置密码";
    //out = jsonWriter.write(jsonRet);
    return status_ok;
}

int CBankFindPasswd::getCacheIdCode(std::string phoneno) {
    std::string key = "CDC|";
    key += phoneno;
    
  /*  int idcode = 0;
    RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
	if (NULL == redisMgr) return idcode;*/

	return RedisLogic::getServerIdCode(phoneno);
}

bool CBankFindPasswd::setUserBackPassword(int mid) 
{
//	MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
//	if(NULL == dbMgr) return false;
	std::map<std::string,std::string> mpInsert;
	mpInsert["bankPWD"] = "";
	if(!SqlLogic::setUserInfo(mid,mpInsert)) return false;
	return true;
}
