#include <stdlib.h>
#include "CGetProductRoomConfig.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Helper.h"
#include "RedisLogic.h"

int CGetProductRoomConfig::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    Json::Value ret;
    //Json::Value result;
    Json::FastWriter jWriter;
    ret["status"] = 0;

    int productid, version;
    try {
        productid = root["productid"].asInt();
        version = root["versions"].asInt();
    } catch (...) {
        ret["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_param_error;
    }
    
    RedisLogic::getProductRoomConfig(productid, version, out);
    
    try {
		out["status"] = 1;
		LOGGER(E_LOG_INFO) << out.toStyledString();
    } catch (const std::exception& e) {
        LOGGER(E_LOG_ERROR) << e.what();
    }

    //out = jWriter.write(result);
    return status_ok;
}
