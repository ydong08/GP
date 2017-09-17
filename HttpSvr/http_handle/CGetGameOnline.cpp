#include <string.h>
#include "CGetGameOnline.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Helper.h"
#include "CConfig.h"


int get_fake_online_num(int product_id, int game_id, int game_level, int real_num)
{
    Json::Value &conf = CConfig::getInstance().m_FakeOnlineConf;
    char game[64] = { 0 };
    sprintf(game , "%d" , game_id);
    if (!conf.isMember(game))
    {
        return real_num;
    }

    char level[64] = { 0 };
    sprintf(level , "%d" , game_level);
    if (!conf[game].isMember(level))
    {
        return real_num;
    }

    int change_range = Json::SafeConverToInt32(conf[game][level]["range"], 5);
    int base_count = Json::SafeConverToInt32(conf[game][level]["base"], 0);
    int limit_count = Json::SafeConverToInt32(conf[game][level]["limit"], 0);
    if (0 >= limit_count)
    {
        return real_num;
    }
    else
    {
        int fake_num = real_num <= limit_count ? real_num : base_count + rand() % change_range + 1;
        LOGGER(E_LOG_DEBUG) <<"product id:"<<product_id<< " , game id: "<<game_id<<" , level:"<<game_level<<" , real online num:"<<real_num<<" , fake num:"<<fake_num;
        return fake_num;
    }
}


int CGetGameOnline::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::FastWriter jWriter;
    //Json::Value ret;
    Json::Value gameids = root["param"]["gameids"];
    out["status"] = 0;
    if (gameids.type() != Json::stringValue) {
        out["msg"] = "游戏id为空";
        //out = jWriter.write(ret);
        return status_param_error;
    }

    int pid = Json::SafeConverToInt32(root["pid"], 0);
    if (pid <= 0)
    {
        out["msg"] = "非法游戏";
        //out = jWriter.write(ret);
        return status_param_error;
    }

    vector<string> gidArr = Helper::explode(gameids.asString().c_str(), ",");
    for (int i = 0; i < gidArr.size(); i++) 
	{
		TrimStr(gidArr[i]);
		if (gidArr[i].empty()) continue; 

        map<string,string> redisRet;
		bool retcode = false;
		if(pid == 1253)
		{
			retcode = ThreadResource::getInstance().getRedisConnMgr()->game()->HGetAll(StrFormatA("Game:%s_1253", gidArr[i].c_str()).c_str(), redisRet);
		}
		else
		{
			retcode = ThreadResource::getInstance().getRedisConnMgr()->game()->HGetAll(StrFormatA("Game:%s", gidArr[i].c_str()).c_str(), redisRet);
		}
        //bool retcode = ThreadResource::getInstance().getRedisConnMgr()->game()->HGetAll(StrFormatA("Game:%s", gidArr[i].c_str()).c_str(), redisRet);
        if (retcode) 
		{
            for (map<string,string>::iterator it = redisRet.begin(); it != redisRet.end(); it++) 
			{
                //out["game"][gidArr[i]][it->first] = atoi(it->second.c_str());
                //LOGGER(E_LOG_DEBUG) <<"product id:"<<pid<< " , game id: "<<gidArr[i]<<" , level:"<<it->first <<" , real online num:"<<it->second;
                out["game"][gidArr[i]][it->first] = get_fake_online_num(pid , atoi(gidArr[i].c_str()) , atoi(it->first.c_str()) , atoi(it->second.c_str()));
            }
        }
    }

    out["status"] = 1;

    //out = jWriter.write(ret);
    return status_ok;
}
