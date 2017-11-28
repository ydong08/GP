#include "CGetFeedBackSet.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Helper.h"
#include "SqlLogic.h"

int CGetFeedBackSet::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
    //Json::FastWriter jWriter;
    int mid, sid, cid, ctype, pid, gameid, productid;
    string mnick, pic, content;
    string strPhoneno;
    out["result"] = 0;
    try {
        mid = root["mid"].asInt();
        sid = root["sid"].asInt();
        cid = root["cid"].asInt();
        ctype = root["ctype"].asInt();
        pid = root["pid"].asInt();
        gameid  = root.isMember("gameid") ? root["gameid"].asInt() : 1;
        productid  = root.isMember("productid") ? root["productid"].asInt() : 0;

        mnick = root["param"].isMember("mnick") ? root["param"]["mnick"].asString() : "";
        strPhoneno = root["param"].isMember("phoneno") ? Helper::filterInput(root["param"]["phoneno"].asString()) : "";
        pic = root["param"].isMember("picurl")  ? root["param"]["picurl"].asString() : "";
        content = root["param"].isMember("content") ? root["param"]["content"].asString(): "";
    } catch (...) {
        out["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_param_error;
    }
    char *ip  = client_ip;

    if(!ctype || !cid  ||!mid || content.empty()){
        //out = jWriter.write(ret);
        return status_account_error;
    }

    ThreadResource::getInstance().getRedisConnMgr()->common()->SET("CustomerNotice", 1, 86400);

    out["result"]  = SqlLogic::feedBack(gameid,cid, sid, pid, ctype, content.c_str(), mid, mnick.c_str(), strPhoneno.c_str(), pic.c_str(),ip,0,productid);
    //out = jWriter.write(ret);
    return status_ok;
}
