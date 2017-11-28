#include <stdlib.h>
#include "CGetRoomConfig.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "CConfig.h"
#include "Helper.h"
#include "RedisLogic.h"

int CGetRoomConfig::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{ 
    Json::Value ret;
    //Json::Value result;
    Json::FastWriter jWriter;
    ret["status"] = 0;

    int gameid, productid, version;
    try {
        gameid   = root["param"]["gameid"].asInt();
        productid = root["productid"].asInt();
        version = root["versions"].asInt();
    } catch (...) {
        ret["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_param_error;
    }
    std::string key;
    std::string many;
	RedisLogic::getRoomConfigKey(gameid, key, many);

    map<int, int> tmp;
    if (!key.empty()) {
        int i = 0;
        for(int id=1;id<=8;id++)
		{
            const char * field_name = "minmoney maxmoney ante tax maxlimit maxallin rase1 rase2 rase3 rase4 magiccoin";
            vector<string> fields = Helper::explode(field_name, " ");
            map<string, string> redisRet;
            bool retCode = ThreadResource::getInstance().getRedisConnMgr()->server()->HMGET(StrFormatA("%s:%d", key.c_str(), id).c_str(), fields, redisRet);
            if (retCode) {
                ret["roomconfig"][i]["roomid"]      = id;
                for(int j = 0; j < fields.size(); j++) 
				{
                    if (redisRet.find(fields[j]) != redisRet.end())
                        ret["roomconfig"][i][fields[j]] = atoi(redisRet[fields[j]].c_str());
                }
                tmp[id] = i;
                i++;
            }
            if (id == 5 && !many.empty()) 
			{
                fields = Helper::explode("ante minmoney", " ");
                retCode = ThreadResource::getInstance().getRedisConnMgr()->server()->HMGET(many.c_str(), fields, redisRet);
                if (retCode) {
                    ret["roomconfig"][i]["roomid"]      = id;
                    ret["roomconfig"][i]["ante"]    = atoi(redisRet["ante"].c_str());
                    ret["roomconfig"][i]["minmoney"]    = atoi(redisRet["minmoney"].c_str());
                    tmp[id] = i;
                    i++;
                }
            }
        }
    }

    const std::string strTb_Product_gamesetting = "game_route.product_gamesetting";
    string sql = StrFormatA("select * from %s where productid=%d and version=%d and gameid=%d order by sort desc,gameid asc", 
        strTb_Product_gamesetting.c_str(), productid, version, gameid);
    MySqlDBAccess* mydb = ThreadResource::getInstance().getDBConnMgr()->DBRouting();
    MySqlResultSet* gameroom = mydb->Query(sql.c_str());
    if (gameroom && gameroom->getRowNum() > 0) {
        for (; gameroom->hasNext();)
            out["roomconfig"].append(ret["roomconfig"][tmp[gameroom->getIntValue("level")]]);
    } else {
        out = ret;
    }

    if(gameroom) delete gameroom;

    out["gameid"] = gameid;
    out["status"] = 1;

    //out = jWriter.write(result);

    return status_ok;
}
