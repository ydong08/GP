#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "CGetFeedBackHistory.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Helper.h"

Json::Value getMyfeed(int gameid, int mid, int productid=0){
    Json::Value _rnt(Json::arrayValue);
    Json::FastWriter jWriter;
    Json::Reader jReader;

    MySqlDBAccess * db = ThreadResource::getInstance().getDBConnMgr()->DBSlave();
    RedisAccess * comm = ThreadResource::getInstance().getRedisConnMgr()->common();
    RedisAccess * commslave = ThreadResource::getInstance().getRedisConnMgr()->common("slave");

    if(!mid || !gameid ){
        return _rnt;
    }

    string value;
    bool retcode = commslave->GET(StrFormatA("FBK|%d|%d", productid,mid).c_str(), value);

    if (!retcode || value == "0") {
        int lastTime = time(NULL) - 15*86400;
        std::string sql = StrFormatA("SELECT * FROM ucenter.uc_feedback WHERE mid=%d AND ctime > %d AND (gameid=%d or productid=%d) ORDER BY ctime DESC",
                                mid, lastTime, gameid, productid);
        MySqlResultSet *feedback = db->Query(sql.c_str());

        sql = StrFormatA("SELECT * FROM ucenter.uc_feedback_reply WHERE fid=0 AND mid=%d AND (gameid=%d or productid=%d) AND rtime > %d ORDER BY rtime ASC LIMIT 20",
                         mid, gameid, productid, lastTime);
        MySqlResultSet *replys = db->Query(sql.c_str());

        int i=0;
        std::vector<int> conSort;
        if(feedback && feedback->getRowNum() > 0){
            for (;feedback->hasNext();) {
                if (i >= 10)
                {
                    break;
                }
                _rnt[i]["feedback"] = feedback->getCharValue("content");
                _rnt[i]["ctime"] = feedback->getIntValue("ctime");
                conSort.push_back(feedback->getIntValue("ctime"));
                _rnt[i]["reply"]    = "";
                _rnt[i]["status"]    = "已提交";
                int status = feedback->getIntValue("status");
                if(status == 1){
                    sql = StrFormatA("SELECT * FROM ucenter.uc_feedback_reply WHERE fid=%d LIMIT 1", feedback->getIntValue("id"));
                    MySqlResultSet * reply = db->Query(sql.c_str());
                    if (reply && reply->hasNext())
                        _rnt[i]["reply"] = reply->getCharValue("content");
                    if(reply) delete reply;
                    _rnt[i]["status"]    = "已回复";
                } else if (status == 3) {
                    _rnt[i]["status"]    = "已处理";
                }
                i++;
            }

            if(replys && replys->getRowNum() > 0){
                for(;replys->hasNext();) {
                    conSort.push_back(replys->getIntValue("rtime"));
                    _rnt[i]["feedback"] = " ";
                    _rnt[i]["reply"]    = replys->getCharValue("content");
                    i ++;
                }
            }

            for (int i = 0; i < _rnt.size(); i++)
                for (int j = i; j < _rnt.size()-1; j++){
                if (conSort[j] > conSort[j+1])
                {
                    int tmp = conSort[j+1];
                    conSort[j+1] = conSort[j];
                    conSort[j] = tmp;
                    _rnt[j].swap(_rnt[j+1]);
                }
            }

        }else{
            if(replys && replys->getRowNum() > 0){
                for(;replys->hasNext();) {
                    _rnt[i]["feedback"] = " ";
                    _rnt[i]["reply"]    = replys->getCharValue("content");
                    i ++;
                }
            }
        }
        if(replys) delete replys;
        if(feedback) delete feedback;

        //LOGGER(E_LOG_INFO) << _rnt.toStyledString();
        if(_rnt.empty()){
            comm->SET(StrFormatA("FBK|%d|%d",productid,mid).c_str(), 0, 3*24*3600);
        }else{
            comm->SET(StrFormatA("FBK|%d|%d",productid,mid).c_str(), Helper::trim(jWriter.write(_rnt).c_str()).c_str(), 3*24*3600);
        }
    } else {
        jReader.parse(value.c_str(),_rnt);
    }

    redisReply * reply = ThreadResource::getInstance().getRedisConnMgr()->ote(mid)->CommandV("hdel OTE|%d feedback", mid);//删除标志
    freeReplyObject(reply);

    return _rnt;
}

int CGetFeedBackHistory::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
    //Json::FastWriter jWriter;
    out["result"] = 0;
    out["msg"] = Json::Value(Json::arrayValue);

    int gameid, mid, productid;
    try {
        gameid = root["gameid"].asInt();
        mid = root["mid"].asInt();
        productid = root["productid"].asInt();
    } catch (...) {
        out["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_param_error;
    }

    Json::Value history = getMyfeed(gameid,mid,productid);

    if(!history.empty()){
        out["result"] = 1;
        out["msg"] = history;
    }
    //out = jWriter.write(ret);
    return status_ok;
}
