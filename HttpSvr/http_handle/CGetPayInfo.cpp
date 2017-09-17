#include "CGetPayInfo.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "JsonValueConvert.h"
#include "StrFunc.h"
#include "TcpMsg.h"

int CGetPayInfo::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value 	 ret;
    //Json::FastWriter write;
    out["result"] = 0;
    out["tips"]   = "订单不存在";
    
    int mid    = Json::SafeConverToInt32(root["mid"]);
    int sid    = Json::SafeConverToInt32(root["sid"]);
    int gameid = Json::SafeConverToInt32(root["gameid"]);
    int cid    = Json::SafeConverToInt32(root["cid"]);
    int pid    = Json::SafeConverToInt32(root["pid"]);
    int ctype  =Json::SafeConverToInt32(root["ctype"]);
    std::string payDealNo = root["param"]["pdealno"].asString();
    
    if (mid <= 0 || payDealNo.empty()) 
	{
        //out = write.write(ret);
        return status_param_error;
    }
    
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["msg"] = "您操作太快了，请休息一会！";
		return status_param_error;
	}

    LOGGER(E_LOG_DEBUG) << root.toStyledString();
    
    Json::Value payInfo;
    std::string tips;
    if (payDealNo.find("wr") != std::string::npos) 
	{
        if (getWithdrawalsByDealNo(payDealNo, payInfo)) 
		{
            if (payInfo["pstatus"] == 4) 
			{
                out["ptype"] = 7;
                tips = "你的申请兑换单号";
                tips += payInfo["pdealno"].asString();
                tips += "审核不通过，请联系客服。";
            } else 
			{
                out["ptype"] = 6;
                tips = "你的申请兑换";
                tips += payInfo["pamount"].asString();
                tips += "元请求已经收到，我们会在十分钟处理到账。";
            }
            out["tips"] = tips;
        } else 
		{
            //out += write.write(ret);
            return status_ok;
        }
    } 
	else 
	{
        if (getPayByDealNo(payDealNo, payInfo)) 
		{
    		out["result"] = 1;
            out["ptype"] = 1;
            tips = "购买成功，得到";
            tips += payInfo["pamount"].asString();
            tips += "金币";
        } else {
            //out = write.write(ret);
            return status_ok;
        }
    }
    out["result"] = 1;
    out["exchangenum"] = payInfo["pexchangenum"].asInt();
    out["viptime"]     = 0;
    out["rool"]        = 0;
    out["roll1"]       = 0;
    out["source"]      = 0;
    out["pdealno"]     = payDealNo;
    out["tips"]        = tips;
    
    CTcpMsg::getMoney(out , mid);
    //out = write.write(ret);
    return status_ok;
}

bool CGetPayInfo::getWithdrawalsByDealNo(std::string payDealNo, Json::Value& info) 
{
    info["pdealno"] = payDealNo;
    
    MySqlDBAccess* pDBAccess = ThreadResource::getInstance().getDBConnMgr()->DBMaster();
    const std::string strTb_Pc_withdrawals = "paycenter.pc_withdrawals";
	std::string strSql = StrFormatA("SELECT pamount, pexchangenum, pstatus FROM %s WHERE pdealno='%s';",
		strTb_Pc_withdrawals.c_str(), payDealNo.c_str());
    
    MySqlResultSet *pResult = pDBAccess->Query(strSql.c_str());
    if (pResult == NULL) {
        return false;
    }
    if (pResult->hasNext()) {
        info["pamount"]      = pResult->getFloatValue(0);
        info["pexchangenum"] = pResult->getFloatValue(1);
        info["pstatus"]      = pResult->getIntValue(2);
        delete pResult;
        return true;
    }
    delete pResult;
    return false;
}

bool CGetPayInfo::getPayByDealNo(std::string payDealNo, Json::Value& info) 
{
    info["pdealno"] = payDealNo;
    
    MySqlDBAccess* pDBAccess = ThreadResource::getInstance().getDBConnMgr()->DBMaster();
	if (NULL  == pDBAccess) return false;

    const std::string strTb_Pc_payment = "paycenter.pc_payment";
	std::string strSql = StrFormatA("SELECT pamount, pexchangenum FROM  %s WHERE pdealno='%s' AND pstatus in (2, 3);", 
        strTb_Pc_payment.c_str(), payDealNo.c_str());
    MySqlResultSet *pResult = pDBAccess->Query((char*)strSql.c_str());
    if (pResult == NULL) 
	{
        return false;
    }
    if (pResult->hasNext()) 
	{
        info["pamount"]      = pResult->getFloatValue(0);
        info["pexchangenum"] = pResult->getIntValue(1);
        delete pResult;
        return true;
    }
    delete pResult;
    return false;
}

