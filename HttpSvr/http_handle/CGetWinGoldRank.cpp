#include "CGetWinGoldRank.h"
#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "threadres.h"
#include "CConfig.h"
#include "SqlLogic.h"
#include "RedisAccess.h"
#include "RedisLogic.h"
#include "Helper.h"

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

	int getPriceByRank(int rank)
	{
		if(rank < 1)
		{
			return 0;
		}

		if(rank == 1)
		{
			return 10888*100;
		}
		else
		{
			return 10088*100 - (rank - 1)*1000*100;
		}
	}


	int getPriceByMoney(int64_t money)
	{
		if (money >= 100000*100)
		{
			return 10888*100;
		}
		else if(money > 90000*100 && money <= 99999*100)
		{
			return 9088*100;
		}
		else if(money > 80000*100 && money <= 89999*100)
		{
			return 8088*100;
		}
		else if(money > 70000*100 && money <= 79999*100)
		{
			return 7088*100;
		}
		else if(money > 60000*100 && money <= 69999*100)
		{
			return 6088*100;
		}
		else if(money > 50000*100 && money <= 59999*100)
		{
			return 5088*100;
		}
		else if(money > 40000*100 && money <= 49999*100)
		{
			return 4088*100;
		}
		else if(money > 30000*100 && money <= 39999*100)
		{
			return 3088*100;
		}
		else if(money > 20000*100 && money <= 29999*100)
		{
			return 2088*100;
		}
		else if(money > 10000*100 && money <= 19999*100)
		{
			return 1088*100;
		}
		else
		{
			return 0;
		}

	}

	int getRankLevelByWinGold(int64_t WinGold)
	{
		if (WinGold >= 100000*100)
		{
			return 1;
		}
		else if(WinGold > 90000*100 && WinGold <= 99999*100)
		{
			return 2;
		}
		else if(WinGold > 80000*100 && WinGold <= 89999*100)
		{
			return 3;
		}
		else if(WinGold > 70000*100 && WinGold <= 79999*100)
		{
			return 4;
		}
		else if(WinGold > 60000*100 && WinGold <= 69999*100)
		{
			return 5;
		}
		else if(WinGold > 50000*100 && WinGold <= 59999*100)
		{
			return 6;
		}
		else if(WinGold > 40000*100 && WinGold <= 49999*100)
		{
			return 7;
		}
		else if(WinGold > 30000*100 && WinGold <= 39999*100)
		{
			return 8;
		}
		else if(WinGold > 20000*100 && WinGold <= 29999*100)
		{
			return 9;
		}
		else if(WinGold > 10000*100 && WinGold <= 19999*100)
		{
			return 10;
		}
		else
		{
			return 0;
		}
	}
}

int CGetWinGoldRank::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
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

	int32_t ActivityBeginTime = 0;
	RedisAccess* rdMoney = RedisLogic::GetRdMoney();
	if(rdMoney != NULL)
	{
		std::string strActivityBeginTime;
		std::string strActivityEndTime;
		if(rdMoney->GET("ActivityBeginTimeStr",strActivityBeginTime) && rdMoney->GET("ActivityEndTimeStr",strActivityEndTime))
		{
			out["startTime"] = strActivityBeginTime.c_str();
			out["endTime"] = strActivityEndTime.c_str();
			ActivityBeginTime = atoi(strActivityBeginTime.c_str());
		}
	}

	RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
	if (redisMgr == NULL)
	{
		LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
		return -1;
	}
	RedisAccess* redis_minfo = redisMgr->minfo();
	//RedisAccess* redisAccess = redisMgr->money("slave");
	RedisAccess* redisAccess = redisMgr->rank(0);
	if(redisAccess == NULL || redis_minfo == NULL)
	{
		out["msg"] = "数据错误";
		return status_ok;;
	}

	std::string ActivityTimeStr = Helper::getTimeStampToYearMonthTime(ActivityBeginTime);
	std::string WinGoldRankStr = StrFormatA("WinGoldRank:%s",ActivityTimeStr.c_str());
	
	//找出自己的分数
	int64_t score = 0;
	//redisReply * pRedisReply1 = redisAccess->CommandV("ZSCORE rankGold %d",uid);
	redisReply * pRedisReply1 = redisAccess->CommandV("ZSCORE %s %d",WinGoldRankStr.c_str(),uid);
	if(pRedisReply1 != NULL && pRedisReply1->len != 0)
	{
		score = atoi64(pRedisReply1->str);
		if (pRedisReply1)
		{
			freeReplyObject(pRedisReply1);
		}
	}
	out["selfWinGold"] = score;

	int myRankLevel = getRankLevelByWinGold(score);
	out["selfLevel"] = myRankLevel;  //再那一档次

	if(myRankLevel != 0)
	{
		int64_t RankLevelMoney = 0;
		if(myRankLevel == 1)
		{
			RankLevelMoney = 9999999900;
		}
		else if(myRankLevel > 1 && myRankLevel <= 10)
		{
			RankLevelMoney = 119999*100 - myRankLevel*10000*100;
		}
		else
		{
			RankLevelMoney = 0;
		}
		//找同档次 比自己高的
		if(RankLevelMoney != 0)
		{
			redisReply * predisReply2 = redisAccess->CommandV("ZREVRANGEBYSCORE %s %lld %lld",WinGoldRankStr.c_str(),RankLevelMoney,score);
			//redisReply * predisReply2 = redisAccess->CommandV("ZREVRANGEBYSCORE rankGold %lld %lld",RankLevelMoney,score);
			if(predisReply2 != NULL && predisReply2->elements != 0)
			{
				//这就是他的名次
				out["selfRank"] = predisReply2->elements;
			}
		}
	}
	else
	{
		out["selfRank"] = 0;
	}
	
	Json::Value info;
	for(int i = 0 ; i < 10;++i)
	{
		info[i]["Level_start"] = 100000*100 - i*10000*100;
		if(i == 0)
		{
			info[i]["Level_end"] = 9999999900;
		}
		else
		{
			info[i]["Level_end"] = 109999*100 - i*10000*100;
		}
		
		if(i == 0)
		{
			info[i]["price"] = 10888*100;
		}
		else
		{
			info[i]["price"] = 10088*100 - i*1000*100;
		}

		int64_t startMoney = 100000*100 - i*10000*100;
		int64_t endMoney = 0;
		if(i == 0)
		{
			endMoney = 9999999900;
		}
		else
		{
			endMoney = 109999*100 - i*10000*100;
		}

		string count;
		count = StrFormatA("peopleInfo%d",i);
		redisReply * predisReply = redisAccess->CommandV("ZREVRANGEBYSCORE %s %lld %lld WITHSCORES LIMIT 0 10",WinGoldRankStr.c_str(),endMoney,startMoney);
		//redisReply * predisReply = redisAccess->CommandV("ZREVRANGEBYSCORE rankGold %lld %lld WITHSCORES LIMIT 0 10",endMoney,startMoney);
		if(predisReply == NULL)
		{
			continue;
		}
		int elementCount = predisReply->elements/2;
		info[i]["peopleCount"] = elementCount;

		if(elementCount == 0)
		{
			out[count] = "";
			continue;
		}
		int LevelPrice = getPriceByRank(i+1);
		
		Json::Value peopleInfo1;
		for(int j = 0; j < elementCount;++j)
		{
			int elementUid = 0;
			if(predisReply->element[j*2])
			{
				elementUid = atoi(predisReply->element[j*2]->str);
			}
			int winMoney = 0;
			if(predisReply->element[j*2+1])
			{
				winMoney = atoi64(predisReply->element[j*2+1]->str);
			}
			
			Json::Value userinfo;
			if(elementUid != 0)
			{
				if (SqlLogic::getOneById(userinfo, elementUid))
				{
					//取头像
					std::string userinfokey = StrFormatA("UCNF%d", elementUid);
					std::string iconid;
					redis_minfo->HGET(userinfokey.c_str(), "iconid", iconid);

					peopleInfo1[j]["icon"] = atoi(iconid.c_str());
					peopleInfo1[j]["mnick"] = userinfo["mnick"].asString();
				}
			}
			else
			{
				peopleInfo1[j]["icon"] = 1;
				peopleInfo1[j]["mnick"] = "";
			}
			
			peopleInfo1[j]["winGold"] = winMoney ;
			peopleInfo1[j]["price"] = LevelPrice;
			peopleInfo1[j]["rank"] = j+1;

		}
		out[count] = peopleInfo1;

		if (predisReply)
		{
			freeReplyObject(predisReply);
		}
	}
	
	out["LevelInfo"] = info;

    return status_ok;
}

