#include "CGetDrawGold.h"
#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "threadres.h"
#include "CConfig.h"
#include "SqlLogic.h"
#include "RedisLogic.h"
#include "Helper.h"

//#include "RedisAccess.h"


int CGetDrawGold::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::FastWriter jWriter;
    //Json::Value ret;



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

	RedisAccess* redisAccess = ThreadResource::getInstance().getRedisConnMgr()->minfo();
	if (!redisAccess)
	{
		LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
		return -1;
	}

	string value;
	int timestamp = Helper::strtotime(Util::formatGMTTime_Date() + " 00:00:00");
	bool reply = redisAccess->GET(StrFormatA("Draw|%d|%d", uid,timestamp).c_str(),value);
	if (reply || value != "")
	{
		out["result"] = 0;
		out["msg"] = "你今天已经抽过了";
		LOGGER(E_LOG_WARNING) << "你今天已经抽过了";
		return status_ok;
	}

	//判断设备
	std::string devno;
	if (!SqlLogic::getDeviceNoByMid(uid, devno))
	{
		//out = writer.write(info);
		out["result"] = 0;
		out["msg"] = "设备错误";
		return status_ok;
	}

	bool reply1 = redisAccess->GET(StrFormatA("Draw|%s|%d", devno.c_str(),timestamp).c_str(),value);
	if (reply1 || value != "")
	{
		out["result"] = 0;
		out["msg"] = "今天该设备已经抽过了";
		LOGGER(E_LOG_WARNING) << "你今天该设备已经抽过了";
		return status_ok;
	}


	Json::Value userInfo;
	if(SqlLogic::getOneById(userInfo, uid))
	{

		//RedisLogic::addWin(userInfo, 300 , MONEY_RECEIVE_BENEFITS, 1, "抽奖领取救济金");

		//身上的金额加上保险柜的余额
		int all_money = Json::SafeConverToInt32(userInfo["money"]) + Json::SafeConverToInt32(userInfo["freezemoney"]);
		if (all_money < 0)
		{
			LOGGER(E_LOG_WARNING) << "User Moeny Is:" << all_money;	//这个用户有问题，直接返回吧
			return status_ok;
		}

		if(all_money >= 200)
		{
		out["result"] = 0;
		out["msg"] = "你的金钱太多";
		return status_ok;
		}

		//抽奖概率 
		int num = rand()%10000;
		int drawMoney = 0;
		int DrawResult = 0;
		if(num < 288)
		{
			DrawResult = 1;
			drawMoney = 500;
		}
		else if(num >= 288 && num < 1288)
		{
			DrawResult = 2;
			drawMoney = 400;
		}
		else if(num >= 1288 && num < 3288)
		{
			DrawResult = 3;
			drawMoney = 300;
		}
		else if(num >= 3288 && num < 6288)
		{
			DrawResult = 4;
			drawMoney = 200;
		}
		else
		{
			DrawResult = 5;
			drawMoney = 100;
		}


		RedisLogic::addWin(userInfo, drawMoney, MONEY_RECEIVE_BENEFITS, 0, "抽奖领取救济金");

		redisAccess->CommandV("set Draw|%s|%d %d",devno.c_str(),timestamp,1);
		redisAccess->CommandV("set Draw|%d|%d %d",uid,timestamp,1);

		out["DrawResult"] = DrawResult;
		out["CurGold"] = all_money + drawMoney;
		out["result"] = 1;
		out["msg"] = "";

	}
	
    return status_ok;
}
