//
//  CReceiveBenefits.cpp
//  HttpSvr
//
//  Created by yuzhan on 2016/11/14.
//
//

#include "CReceiveBenefits.h"
#include "../threadres.h"
#include "MySqlDBAccess.h"
#include "Helper.h"
#include "Util.h"
#include "SqlLogic.h"

namespace
{
    int insert_data_to_db(const std::string& table, const std::map<std::string, std::string>& data)
    {
		if (0 == data.size()) return 0;

		std::string strKeyValue;
        for (auto it = data.begin(); it != data.end(); it++)
        {
			if (!strKeyValue.empty()) strKeyValue+=", ";
            strKeyValue += StrFormatA("%s='%s'", it->first.c_str(), it->second.c_str());
		}
		std::string sql = StrFormatA("insert into %s set %s", table.c_str(), strKeyValue.c_str());
        LOGGER(E_LOG_INFO) << sql;
        MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
        return dbMgr->DBMaster()->ExeSQL(sql.c_str());
    }
}

int CReceiveBenefits::do_request(const Json::Value &root, char *client_ip, HttpResult &out)
{
	Json::FastWriter writer;
	//Json::Value info;
	out["status"] = 0;
	if (root["mid"].isNull() || root["ctype"].isNull() || root["pid"].isNull() || root["param"].isNull() || root["gameid"].isNull() || 
		root["sid"].isNull() || root["param"]["type"].isNull() || root["param"]["username"].isNull())
	{
		out["msg"] = "参数不正确";
		return status_argment_access;
	}

	int mid = Json::SafeConverToInt32(root["mid"]);
	int ctype = Json::SafeConverToInt32(root["ctype"]);
	int pid = Json::SafeConverToInt32(root["pid"]);
	int type = Json::SafeConverToInt32(root["param"]["type"]);
	int gameid = Json::SafeConverToInt32(root["gameid"]);
	int sid = Json::SafeConverToInt32(root["sid"]);
	std::string username = root["param"]["username"].asString();
	out["tips"] = "当前不可领";
	if (mid <= 0)
	{
		out["msg"] = "mid参数错误";
		return status_ok;
	}
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}

	MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
	if(NULL == dbMgr)
	{
		LOGGER(E_LOG_ERROR) << "getDBConnMgr() retturn is NULL";
		//out = writer.write(info);
		return status_check_error;
	}
	MySqlDBAccess* dbSlave = dbMgr->DBSlave();
	if (NULL == dbSlave)
	{
		LOGGER(E_LOG_ERROR) << "getDBConnMgr() retturn is NULL";
		//out = writer.write(info);
		return status_check_error;
	}

	std::string devno;
	if (!SqlLogic::getDeviceNoByMid(mid, devno))
	{
		//out = writer.write(info);
		return status_ok;
	}
	std::vector<int> vec;
	if (!SqlLogic::getMidArrayByDeviceNo(devno, vec))
	{
		//out = writer.write(info);
		return status_ok;
	}
	std::string strTableName = "ucenter.uc_system_recharge_other";
	int start = Helper::strtotime(Util::formatGMTTime_Date() + " 00:00:00");
	int end = Helper::strtotime(Util::formatGMTTime_Date() + " 23:59:59");
	if (1 == type)
	{
		if (vec.size() > 0)
		{
			std::string mid_str = Helper::implode(",", vec);
			std::string strSql = StrFormatA("select mid from %s where mid in (%s) and pstatus=1 and ptime >= %d and ptime <= %d limit 1", strTableName.c_str(), mid_str.c_str(), start, end);
			auto result = dbSlave->Query(strSql.c_str());
			int nDbMid = -1;
			if (result && result->hasNext())
			{
				nDbMid = result->getIntValue("mid");
			}
			if(result) delete result;
			std::string strUserName;
			if (nDbMid > 0)
			{
				Json::Value jvUserInfo;
				if(SqlLogic::getOneById(jvUserInfo,nDbMid))
				{
					if (Json::SafeConverToInt32(jvUserInfo["sid"]) == 100) 
					{
						strUserName = "游客";
					} 
				}
				if (strUserName.empty())
				{
					SqlLogic::getUserNameBySitemid(Json::SafeConverToInt32(jvUserInfo["sitemid"]),Json::SafeConverToInt32(jvUserInfo["sid"]),strUserName);
				}
				if (nDbMid == mid)
				{
					//判断还剩多少金币
					std::string totla_query = StrFormatA("select SUM(pexchangenum) as money from %s where mid=%d and pstatus=1 and ptime >= %d and ptime <= %d", strTableName.c_str(), mid, start, end);
					result = dbSlave->Query(totla_query.c_str());
					int nReceivedMoney = 0; //已经领取的钱
					if (result && result->hasNext())
					{
						nReceivedMoney = result->getIntValue("money");
						if(nReceivedMoney < 0) nReceivedMoney = 0;
					}
					if(result) delete result;
					out["lastmoney"] = 600 - nReceivedMoney;
				}
				else
				{
					out["lastmoney"] = 0;
				}
			}
			else
			{
				out["lastmoney"] = 600;
			}
			out["lastname"] = strUserName;	//strUserName.empty() ? " " : 
			out["tips"] = "";
			out["status"] = 2;
		}

		return status_ok;
	}

	Json::Value userinfo;
	if (!SqlLogic::getOneById(userinfo, mid))
	{
		//out = writer.write(info);
		return status_ok;
	}
	//身上的金额加上保险柜的余额
	int all_money = Json::SafeConverToInt32(userinfo["money"]) + Json::SafeConverToInt32(userinfo["freezemoney"]);
	if (all_money < 0)
	{
		LOGGER(E_LOG_WARNING) << "User Moeny Is:" << all_money;	//这个用户有问题，直接返回吧
		return status_ok;
	}

	if (vec.size() > 0)
	{
		std::string mid_str = Helper::implode(",", vec);
		std::string strSql = StrFormatA("select mid from %s where mid in (%s) and pstatus=1 and ptime >= %d and ptime <= %d limit 1", strTableName.c_str(), mid_str.c_str(), start, end);
		auto result = dbSlave->Query(strSql.c_str());
		int nDbMid = -1;
		if (result && result->hasNext())
		{
			nDbMid = result->getIntValue("mid");
		}
		if(result) delete result;
		if (nDbMid > 0 && nDbMid != mid)
		{
			out["tips"] = "该账号不能领取";
			return status_ok;
		}
	}

	//判断还剩多少金币
	std::string totla_query = StrFormatA("select SUM(pexchangenum) as money from %s where mid=%d and pstatus=1 and ptime >= %d and ptime <= %d", strTableName.c_str(), mid, start, end);
	auto result = dbSlave->Query(totla_query.c_str());
	int nReceivedMoney = 0; //已经领取的钱
	if (result && result->hasNext())
	{
		nReceivedMoney = result->getIntValue("money");
		if(nReceivedMoney < 0) nReceivedMoney = 0;
	}
	if(result) delete result;


    if ( all_money >= 200 )
    {
        if (nReceivedMoney == 0)
        {
            out["tips"] = "您今天还能领取2次救济金（金币不足2.00时可领取）";
        }
        else if (nReceivedMoney>0 && nReceivedMoney <= 300)
        {
            out["tips"] = "您今天还能领取1次救济金（金币不足2.00时可领取）";
        }
        else if (nReceivedMoney > 300)
        {
            out["tips"] = "您今天的2次救济金已经领完了，明天还能继续领取，祝您东山再起";
        }
        
        return status_ok;
    }
    else
    {
        if (nReceivedMoney > 300)
        {
            out["tips"] = "您今天的东山再起机会已经领完了，明天领吧";
            return status_ok;
        }
    }

    int nLastMoney = 0;
    if (nReceivedMoney <= 0) //如果是第一次领取，领取后还有一次领取3元的机会
    {
        nLastMoney = 300;
    }

	//int nGold = 300 - nReceivedMoney;
	//if (nGold + all_money > 300)
	//{
	//	nGold = 300 - all_money;
	//}

	//if ( nGold < 1 ) 
	//{
	//	LOGGER(E_LOG_WARNING) << "nGold Is:" << nGold << ",nReceivedMoney Is:" << nReceivedMoney << ",all_money Is:" << all_money;	//这个用户有问题，直接返回吧
	//	out["tips"] = "当前不可领";
	//	return status_ok;
	//}
	//int nLastMoney = 300 - nReceivedMoney - nGold;

	std::map<std::string, std::string> mp;
	mp["userid"] = "1";						//C++ ucenter 是不是可以换成2
	mp["mid"] = StrFormatA("%d", mid);
	mp["pmoneynow"] = Json::SafeConverToStr(userinfo["money"]);
	mp["ip"] = client_ip;
	mp["type"] = "1";
	mp["ctype"] = StrFormatA("%d", ctype);
	//mp["pmoneyafter"] = StrFormatA("%d", Json::SafeConverToInt32(userinfo["money"]) + nGold);
	//mp["ptype"] = StrFormatA("%d", (nGold >= 0) ? 1 : 2);
	//mp["pexchangenum"] = StrFormatA("%d", nGold);	//小于0的情况已经返回了，不需要判断abs
    mp["pmoneyafter"] = StrFormatA("%d" , Json::SafeConverToInt32(userinfo["money"]) + 300);
    mp["ptype"] = StrFormatA("%d" , 1);
    mp["pexchangenum"] = StrFormatA("%d" , 300);
	std::string deal_no = StrFormatA("ot%d%d", time(NULL), Util::rand_range(1000, 10000));
	mp["pdealno"] = deal_no;
	mp["ptime"] = StrFormatA("%d", Util::getGMTTime_S());
	mp["remark"] = "救济金领取";
	mp["pstatus"] = "0";
	if (insert_data_to_db(strTableName, mp) <= 0)
	{
		out["tips"] = "操作失败";
		//out = writer.write(info);
		return status_ok;
	}
	
	Json::Value param = root;
	param["pdealno"] = mp["pdealno"];
	param["time"] = (int64_t)time(NULL);
	if (SqlLogic::paySystemRechargeOther(param))
	{
		out["tips"] = "领取成功，祝您东山再起";
		out["lastmoney"] = nLastMoney;
		out["lastname"] = username;
		out["status"] = 1;
	}
	return status_ok;
}
