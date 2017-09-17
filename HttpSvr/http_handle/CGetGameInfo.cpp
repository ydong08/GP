#include "CGetGameInfo.h"
#include "json/json.h"
#include "Logger.h"
#include "MySqlDBAccess.h"
#include "../threadres.h"
#include "RedisAccess.h"
#include "../CConfig.h"

#include "SqlLogic.h"

namespace
{
    bool GetOTEInfo(int uid, int gameid, Json::Value& info)
    {
        RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
        if (redisMgr == NULL)
        {
            info["msg"] = "连接reids出错";
            return false;
        }
        RedisAccess* redisAccess = redisMgr->ote(uid);
        if (redisAccess == NULL)
        {
            info["msg"] = "连接reids出错";
            return false;
        }
        std::string hash = StrFormatA("OTE|%d", uid);
        std::vector<std::string> fields;
        fields.push_back("wi");
        fields.push_back("ls");
        fields.push_back("da");
        std::map<std::string, std::string> ret;
        if (!redisAccess->HMGET(hash.c_str(), fields, ret))
        {
            info["msg"] = "连接reids出错";
            return false;
        }
        info["wintimes"] = atoi(ret["wi"].c_str());
        info["losetimes"] = atoi(ret["ls"].c_str());
        info["drawtimes"] = atoi(ret["da"].c_str());
        info["result"] = 1;
        
        return true;
    }
}

int CGetGameInfo::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    int gameid = Json::SafeConverToInt32(root["gameid"]);
    int mid = Json::SafeConverToInt32(root["mid"]);
    //Json::Value ret;
    //Json::FastWriter writer;
    if (mid <= 0 || gameid < 0)
    {
        out["result"] = 0;
        LOGGER(E_LOG_WARNING) << "param invalid, gameid = " << gameid << "|mid = " << mid;
        //out = writer.write(ret);
        return 0;
    }
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["result"] = -1;
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}
  //  MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
    if (!SqlLogic::GetGameInfo(mid, out))
    {
        out["result"] = 0;
        LOGGER(E_LOG_WARNING) << "get game info failed";
        //out = writer.write(ret);
        return 0;
    }
    //out = writer.write(ret);
    return status_ok;
}
