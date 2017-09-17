#include "CGetTaskInfo.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Util.h"
#include "Helper.h"

int taskNum(int mid, int gameid, int level, int productid) {
    int cnt = 0;
    if (!mid || !gameid) {
        return 0;
    }

    string table = StrFormatA("logchip.log_member%d", productid%10);
    if (productid % 10 == 0) {
        table = "logchip.log_member10";
    }

	int start = Helper::strtotime(Util::formatGMTTime_Date() + " 00:00:00");
	int end = Helper::strtotime(Util::formatGMTTime_Date() + " 23:59:59");
    string sql = StrFormatA("SELECT count( mid ) total FROM %s WHERE mid =%d and gameid = %d and level =%d and pid=%d and stime>%d and stime < %d",
                        table.c_str(), mid, gameid, level, productid, start, end);
    MySqlResultSet * row = ThreadResource::getInstance().getDBConnMgr()->DBSlave()->Query(sql.c_str());
    if (row && row->hasNext())
        cnt = row->getIntValue("total");
    delete row;

    return cnt;
}

int CGetTaskInfo::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	//Json::Value result;
	Json::FastWriter jWriter;
	out["result"] = 0;
	if (root["mid"].isNull() || root["productid"].isNull())
	{
		out["result"] = "参数不正确";
		return status_argment_access;
	}
    int mid = Json::SafeConverToInt32(root["mid"]);
	if (mid < 1)
	{
		out["result"] = "参数不正确";
		return status_argment_access;
	}
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_argment_access;
	}

    int productid = Json::SafeConverToInt32(root["productid"]);
    RedisAccess * commslave = ThreadResource::getInstance().getRedisConnMgr()->common("slave");
    RedisAccess * comm = ThreadResource::getInstance().getRedisConnMgr()->common();
	if (NULL == commslave || comm)
	{
		out["result"] = "连接redis出错";
		//out = jWriter.write(result);
		return status_check_error;
	}

    vector<string> fields = Helper::explode("task_1 task_2 task_3 task_4 task_5", " ");
    map<string, string> redisRet;
    bool retcode = commslave->HMGET("DailyTaskConfig", fields, redisRet);//获取任务信息
    if (!retcode) {
        redisRet["task_1"] ="1|3|2|5|斗地主1元场游戏5局|10";
        redisRet["task_2"]="2|7|3|5|炸金花5元场游戏5局|10";
        redisRet["task_3"]="3|7|4|5|炸金花10元场游戏5局|10";
        redisRet["task_4"]="4|2|3|5|疯狂斗牛5元场游戏5局|10";
        redisRet["task_5"]="5|2|4|5|疯狂斗牛10元场游戏5局|10";
        comm->HMSET("DailyTaskConfig", redisRet);
    }

    for (map<string, string>::iterator it = redisRet.begin(); it != redisRet.end(); it++)
	{
        Json::Value task;
        //任务id |游戏id|房间等级|任务局数|任务描述|任务奖励
        const char * keyarr[] = {"taskid", "gameid", "level", "number", "desc", "reward"};
        vector<string> taskArr = Helper::explode(it->second.c_str(), "|");
        for (int i=0; i < taskArr.size(); i++) {
            task[keyarr[i]] = taskArr[i];
        }

        string s_taskstatus = StrFormatA("taskstatus_%d_%s_%s", mid, task["taskid"].asString().c_str(), Util::formatGMTTime_Date().c_str());
        string value;
        retcode = commslave->HGET("DailyTaskConfig", s_taskstatus.c_str(), value);
        int taskstatus = 0;
        if (retcode)
            taskstatus = atoi(value.c_str());

        if(taskstatus == 1 || taskstatus == 2){ //1已完成2已领取
            task["status"] = taskstatus;
            task["schedule"] = "";
        }else{
            int number = taskNum(mid, atoi(task["gameid"].asString().c_str()),atoi(task["level"].asString().c_str()),productid);

            if(number >= 5){
                task["status"] = 1;
                task["schedule"] = "";
                retcode = comm->HSET("DailyTaskConfig",
                                     s_taskstatus.c_str(),
                                     "1");
//                if (retcode)
//                    comm->EXPIRE("DailyTaskConfig", 86400);
            }else{
                task["status"] = 0;
                task["schedule"] = number;
            }
        }
        out.append(task);
    }
	out["result"] = 1;
    //out = jWriter.write(result);
    return status_ok;
}
