#include "CGetGameResource.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "CConfig.h"
#include "Helper.h"

int CGetGameResource::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
    //Json::FastWriter jWriter;
    string gameids;
    int productid, version;
    out["status"] = 0;

    try {
        gameids= root["param"]["gameids"].asString();
        productid= root["productid"].asInt();
        version  = root["versions"].asInt();
    } catch (...) {
        out["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_ok;
    }

    if (gameids.empty()) {
        out["msg"] = "游戏id为空";
        //out = jWriter.write(ret);
        return status_ok;
    }

    vector<string> gidsArr = Helper::explode(gameids.c_str(), ",");
    for (int i =0; i < gidsArr.size(); i++)
        out["game"][gidsArr[i]] = StrFormatA("%sgames/%d/%s/project_old.manifest", CConfig::getInstance().resDomain.c_str(), productid, gidsArr[i].c_str()).c_str();

    //获取Tips信息
    const std::string strTb_Tips = "zq_admin.tips";
    string sql = StrFormatA("select * from %s where productid=%d and status=1 order By sort desc", 
        strTb_Tips.c_str(), productid);
    MySqlResultSet * tips = ThreadResource::getInstance().getDBConnMgr()->DBSlave()->Query(sql.c_str());
    Json::Value result_rand;
    if (tips && tips->getRowNum() > 0) {
        for(;tips->hasNext();)
            result_rand.append(tips->getCharValue("content"));
    }
    delete tips;

    out["tips"]=result_rand;
    out["rollmsg"]="欢迎来到<color=0x2cda5dFF>澳门大富翁</color>，祝您游戏愉快，日进斗金！";
	out["qq"] = "mqqwpa://im/chat?chat_type=wpa&uin=3450271624&version=1&src_type=web&web_src=oicqzone.com";
	out["mychat"] = "http://116.31.115.226:8431/welive1618.php";
    out["ismychat"] = 1;
    out["qqnew"]    = "http://139.129.201.216:8011/qq.html";

    // 获取后台设置的 ismychat qq qqnew rollmsg 
    RedisAccess* rdCommon = ThreadResource::getInstance().getRedisConnMgr()->common("slave");
    std::string cache_key = StrFormatA("GameResource|%d", productid);
    std::vector<std::string> vsFields;
    vsFields.push_back("ismychat");
    vsFields.push_back("mychat");
    vsFields.push_back("qq");
    vsFields.push_back("qqnew");
    vsFields.push_back("rollmsg");
    std::map<std::string , std::string> mpRet;
    if ( rdCommon->HMGET(cache_key.c_str() , vsFields , mpRet) )
    {
        out["ismychat"] = atoi(mpRet["ismychat"].c_str());
        out["mychat"]   = mpRet["mychat"];
        out["qqnew"]    = mpRet["qqnew"];
        out["qq"]       = mpRet["qq"];
        out["rollmsg"]  = mpRet["rollmsg"];
    }
    else
    {
		LOGGER(E_LOG_INFO) << "read common redis return false[ip:" << rdCommon->GetIp() << ",port:" << rdCommon->GetPort() << "]"<<  " ; HGET Key Is : " << cache_key;
    }

    out["status"] = 1;

    //out = jWriter.write(ret);
    return status_ok;
}
