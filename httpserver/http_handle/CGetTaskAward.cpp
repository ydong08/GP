#include "CGetTaskAward.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Util.h"
#include "Helper.h"
#include "TcpMsg.h"

int setBankLog(int mid, int amount, int money_now, int freezemoney, int gameid, int type, int productid = 1,
               int tomid = 0) {
    int ctime = time(NULL);
    const std::string strTb_Log_bank  = StrFormatA("logchip.log_bank%d", productid % 10);
    std::string sql = StrFormatA("INSERT INTO %s SET mid=%d,type=%d,productid=%d,amount=%d,money=%d,freezemoney=%d,tomid=%d,ctime=%d" , 
                       strTb_Log_bank.c_str(), mid, type, productid, amount, money_now, freezemoney, tomid, ctime);
    return ThreadResource::getInstance().getDBConnMgr()->DBMaster()->ExeSQL(sql.c_str());
}

int CGetTaskAward::do_request(const Json::Value &root, char *client_ip, HttpResult &out) {
    return 0;
    //Json::Value result;
    Json::FastWriter jWriter;

    int gameid, mid, productid, taskid;

    try {
        gameid = root["gameid"].asInt();
        mid = root["mid"].asInt();
        productid = root["productid"].asInt();
        taskid = root["param"]["taskid"].asInt();
    } catch (...) {
        out["status"] = 0;
        out["msg"] = "缺少参数";
        //out = jWriter.write(result);
        return status_argment_access;
    }
	if (mid < 1)
	{
		out["status"] = 0;
		out["msg"] = "参数不正确";
		return status_argment_access;
	}
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["status"] = 0;
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}

    out["taskid"] = taskid;
    RedisAccess *commslave = ThreadResource::getInstance().getRedisConnMgr()->common("slave");
    vector<string> fields;
    string s_taskstatus = StrFormatA("taskstatus_%d_%d_%s", mid, taskid, Util::formatGMTTime_Date().c_str());
    string s_taskinfo = StrFormatA("task_%d", taskid);
    fields.push_back(s_taskstatus);
    fields.push_back(s_taskinfo);
    map<string,string> value;
    commslave->HMGET("DailyTaskConfig", fields, value);
    std::vector<string> task;
    if (value.find(s_taskinfo) != value.end()) {
        task = Helper::explode(value[s_taskinfo].c_str(), "|");
    }
    int taskstatus = 0;
    if (value.find(s_taskstatus) != value.end())
        taskstatus = atoi(value[s_taskstatus].c_str());

    if (taskstatus == 1 && task.size() >= 6) {
        Json::Value gameInfo;
        int money = atoi(task[5].c_str());
        bool bRet = CTcpMsg::setBank(gameInfo, mid, money);
        if (!bRet) {
            out["status"] = 0;
            out["msg"] = "领取奖励失败";
        } else {
            setBankLog(mid, money, Json::SafeConverToInt32(gameInfo["money"]), Json::SafeConverToInt32(gameInfo["freezemoney"]), gameid, 3, productid);
            RedisAccess * ucenter = ThreadResource::getInstance().getRedisConnMgr()->ucenter();
            bool retcode = ucenter->HSET("DailyTaskConfig",
                                         s_taskstatus.c_str(),
                                         "2");
//            if (retcode)
//                ucenter->EXPIRE("DailyTaskConfig", 86400);
            out["status"] = 1;
        }
    } else {
        out["status"] = 0;
        out["msg"] = "未满足要求或已领取";
    }

    //out = jWriter.write(result);
    return status_ok;
}
