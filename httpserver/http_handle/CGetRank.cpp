#include "CGetRank.h"
#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "threadres.h"
#include "CConfig.h"
#include "SqlLogic.h"

namespace
{
    /**
 * 取出头像
 */
    string getIcon(int mid, string size = "middle", int gameid = 1, int sex = 1) {
        int subname = mid % 10000;

        string icon, iconVersion;
        bool bRet = ThreadResource::getInstance().getRedisConnMgr()->ote(mid)->HGET(StrFormatA("OTE|%d", mid).c_str(), "iconVersion", iconVersion);
        if (bRet)
            icon = StrFormatA("%s/data/icon/%d/%d_%s.jpg?v=%s", CConfig::getInstance().iconDomain.c_str(), subname, mid , size.c_str(), iconVersion.c_str());

        return icon;
    }
    
    static const std::string RankKey = "rank|0|";
    std::string GetRankKey(int productid)
    {
        return StrFormatA("rank|%d|%s", productid, Util::formatGMTTime_Date().c_str());
    }
    
    // 获取前num的排名
    void getTopRank(int productid, int num, Json::Value &info)
    {
        RedisAccess* redisAccess = ThreadResource::getInstance().getRedisConnMgr()->ucenter("slave");
        std::vector<RedisAccess::DataScore> vec;
        if (!redisAccess->ZREVRANGEWITHSCORE(GetRankKey(0).c_str(), 0, num-1, vec) || vec.empty())
            return;
//        info.resize(vec.size());
        MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
        int idx = 0;
        for (int i = 0; i < vec.size(); i++)
        {
            int mid = atoi(vec[i].key.c_str());
            Json::Value userinfo;
            if (SqlLogic::getOneById(userinfo, mid)) {
                info[idx]["sid"] = atoi(userinfo["sid"].asString().c_str());
                info[idx]["mid"] = mid;
                info[idx]["trend"] = 2;
                info[idx]["icon"] = getIcon(mid);
                info[idx]["mnick"] = userinfo["mnick"].asString();
                info[idx]["rank"] = idx+1;
                info[idx]["score"] = vec[i].score;
                idx++;
            }
        }
    }
    
    // 获取自己的排名
    void getUserRank(int uid, int productid, Json::Value &info)
    {
        std::string rankkey = GetRankKey(0);
        RedisAccess* redisAccess = ThreadResource::getInstance().getRedisConnMgr()->ucenter("slave");
        int rank = 0;
        if (!redisAccess->ZREVRANK(rankkey.c_str(), uid, rank))
            return;
        int score = 0;
        if (!redisAccess->ZSCORE(rankkey.c_str(), uid, score))
            return;
        MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
        Json::Value userinfo;
        SqlLogic::getOneById(userinfo, uid);
        info["mid"] = uid;
        info["icon"] = "";
        info["rank"] = rank + 1;
        info["score"] = score;
        info["mnick"] = userinfo["mnick"].asString();
        info["sid"] = atoi(userinfo["sid"].asString().c_str());

    }
}

int CGetRank::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::FastWriter jWriter;
    //Json::Value ret;
    int uid, productid;
    try {
        uid = root["mid"].asInt();
        productid = root["productid"].asInt();
    } catch (...) {
        out["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_param_error;
    }
	if (uid < 1)
	{
		out["msg"] = "参数不正确";
		return status_param_error;
	}
	CAutoUserDoing userDoing(uid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}

    out["result"] = 0;
    out["rank"] = 0;
    Json::Value topRank;
    getTopRank(productid, 100, topRank);
    bool contained = false;
    if (!topRank.isArray() || topRank.empty())
    {
        LOGGER(E_LOG_WARNING) << "no rank data in redis";
        //out = jWriter.write(ret);
        return status_ok;
    }
    bool inc = false;
    for (auto it = topRank.begin(); it != topRank.end(); it++)
    {
        Json::Value v = *it;
        if (Json::SafeConverToInt32(v["mid"]) == uid)
        {
            contained = true;
            break;
        }
    }
    //ULOGGER(E_LOG_WARNING, uid) << "contained:" << contained << " topRank count:" << topRank.size() << jWriter.write(topRank);
    if (!contained)
    {
        Json::Value userRank;
        getUserRank(uid, productid, userRank);
        if (!userRank.empty()) {
            int count = topRank.size();
            topRank.resize(count+1);
            topRank[count] = userRank;
        }
    }
    if (!topRank.isArray() || topRank.empty())
    {
        LOGGER(E_LOG_ERROR) << "get rank form redis failed";
        out["msg"] = "无法获取排行榜数据";
        //out = jWriter.write(ret);
        return status_ok;
    }
    out["result"] = 1;
    out["resetTime"] = "24:00";
    out["info"] = topRank;
    //out = jWriter.write(ret);

    return status_ok;
}
