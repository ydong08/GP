#include "CGetRankAwardRules.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"

#include "RedisAccess.h"
#include "MySqlDBAccess.h"


int CGetRankAwardRules::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value result;

    string rules;
	std::string strRulKey = "CPP|rankAward";
    RedisAccess* ucenter = ThreadResource::getInstance().getRedisConnMgr()->ucenter();
    bool retcode = ucenter->GET(strRulKey.c_str(), rules);
    if(retcode)
	{
		Json::Value jvRules;
		Json::Reader jrRules;
		if (jrRules.parse(rules,jvRules))
		{
			out["status"] = 1;
			out["mag"] = "";
			out["rules"] = jvRules;
		}
		else
		{
			out["status"] = 0;
			out["mag"] = "缓存信息解析出错";
			out["rules"] = "";
		}
    }
	else
	{
        string query = StrFormatA("select start,end,money from ucenter.uc_rankaward group by sort desc");
        MySqlResultSet *res = ThreadResource::getInstance().getDBConnMgr()->DBSlave()->Query(query.c_str());
        int i = 0;
        string key = StrFormatA("%d", i++);
        if (res && res->getRowNum() > 0) {
            for (; res->hasNext(); ) {
                if(res->getIntValue("start") == res->getIntValue("end")){
                    out["rules"][key]["start"] = res->getIntValue("start");
                }else{
                    out["rules"][key]["start"] = StrFormatA("%d-%d", res->getIntValue("start"), res->getIntValue("end"));
                }
                out["rules"][key]["money"] = res->getIntValue("money");
                key = StrFormatA("%d", i++);
            }
        }
		if(res) delete res;

        out["status"] = 1;
        out["msg"] = "";
        out["rules"]["score"] = "充值1元=1积分游戏胜利所交的服务费；举例，用户当局获胜所交服务费3.46，那么他的积分就是3.46";
		out["rules"]["tip"] = "0点结束，并清零积分，积分可以增加排行名次，名次可以领取金币！";
		Json::FastWriter jWriter;
        rules = jWriter.write(out["rules"]);
        ucenter->SET(strRulKey.c_str(), rules.c_str());
    }
    //out = jWriter.write(result);
    return status_ok;
}
