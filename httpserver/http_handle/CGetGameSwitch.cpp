#include "CGetGameSwitch.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Lib_Ip.h"
#include <set>
#include "CConfig.h"
#include "RedisLogic.h"
#include "Helper.h"

int CGetGameSwitch::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
    
	//LOGGER(E_LOG_WARNING) <<"Begin CGetGameSwitch";
	//int i = 1;
	//MySqlDBAccess* dbMaster = ThreadResource::getInstance().getDBConnMgr()->DBMaster();
	//if(dbMaster != NULL)
	//{
	//	std::string sql = StrFormatA("SELECT * FROM logchip.log_member3 where mid = 22");

	//	MySqlResultSet* res = dbMaster->Query(sql.c_str());

	//	//for (; res->hasNext();) 
	//	{
	//		LOGGER(E_LOG_WARNING) <<"Count:" << i;
	//		if (res && res->getRowNum() > 0) 
	//		{
	//			LOGGER(E_LOG_WARNING) <<"111111111111111111111111111111";
	//			LOGGER(E_LOG_WARNING) <<res->getIntValue("id");
	//			LOGGER(E_LOG_WARNING) <<res->getIntValue("gameid");
	//			LOGGER(E_LOG_WARNING) <<res->getIntValue("mid");
	//			LOGGER(E_LOG_WARNING) <<res->getIntValue("money");
	//			LOGGER(E_LOG_WARNING) <<"2222222222222222222222222222222222222";
	//			int LandMoney = res->getIntValue("mid");
	//			//LandCount = res2->getIntValue("LandCount");
	//			LOGGER(E_LOG_WARNING) << "LandMoney" << LandMoney;
	//		}
	//		i++;
	//	}
	//}

    //Json::FastWriter jWriter;
    out["status"] = 1;
	CConfig& config  = CConfig::getInstance();
	Json::Value& paymentInfo = config.m_jvPaymentInfo;
    out["phoneRegister"] = paymentInfo["phoneRegister"]; 
	out["otherpay"] = paymentInfo["otherpay"];

	out["pmode"] = paymentInfo["pmode"]; 
	out["paytype"] = paymentInfo["paytype"];
	out["othertype"] = paymentInfo["othertype"]; 
	out["bank"] = paymentInfo["bank"];

	out["bankinfo"]["bankno"] = paymentInfo["bankinfo"]["bankno"];
	out["bankinfo"]["bankname"] = paymentInfo["bankinfo"]["bankname"];
	out["bankinfo"]["username"] = paymentInfo["bankinfo"]["username"];
    out["qrcodeinfo"][0] = "";
    out["qrcodeinfo"][1] = "";
	
	RedisAccess* rdMoney = RedisLogic::GetRdMoney();
	if(rdMoney != NULL)
	{
		std::string strActivityOpen;
		if (rdMoney->GET("ActivityOpen",strActivityOpen))
		{
		    uint32_t ActivityOpen = atoi(strActivityOpen.c_str());
			//为1 是打开
			if(ActivityOpen == 1)
			{
				out["activityOpen"] = 1;
				std::string strActivityBeginTime;
				std::string strActivityEndTime;
				if(rdMoney->GET("ActivityBeginTime",strActivityBeginTime) && rdMoney->GET("ActivityEndTime",strActivityEndTime))
				{
					int32_t ActivityBeginTime = atoi(strActivityBeginTime.c_str());
					int32_t ActivityEndTime = atoi(strActivityEndTime.c_str());
					int CurTime = time(NULL);
					if(CurTime >= ActivityBeginTime && CurTime <= ActivityEndTime)
					{
						out["activityOpen"] = 1;
					}
					else
					{
						out["activityOpen"] = 0;
					}
				}
			}
			else
			{
				out["activityOpen"] = 0;
			}
		}

		std::string strDrawOpen;
		if (rdMoney->GET("DrawOpen",strDrawOpen))
		{
			uint32_t DrawOpen =	atoi(strDrawOpen.c_str());
			if(DrawOpen == 1)
			{
				out["darwOpen"] = 1;
			}
			else
			{
				out["darwOpen"] = 0;
			}
		}
	}
	else
	{
		out["darwOpen"] = 1;
	}

    int ctype, productid, version;
    try {
        ctype = root["ctype"].asInt();
        productid = root["productid"].asInt();
        version = root["versions"].asInt();
    } catch (...) {
        out["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        LOGGER(E_LOG_INFO) << "client 参数不正确!!! pmode:" << out["pmode"].asString() << " ;otherpay: " << out["otherpay"].asString()<<" othertype:"<<out["othertype"].asString();
        return status_param_error;
    }

    std::string strKey = StrFormatA("GameSwitch|%d" , productid);
    std::string strField = StrFormatA("%d" , version);
    std::string strRet;
	RedisAccess* rdCommon = RedisLogic::GetRdCommon("slave");
	if (NULL == rdCommon)
	{
		out["msg"] = "缓存服务器出错了";
        LOGGER(E_LOG_INFO) << "client 缓存服务器出错了!!! pmode:" << out["pmode"].asString() << " ;otherpay: " << out["otherpay"].asString()<<" othertype:"<<out["othertype"].asString();
		//return status_param_error;
	}
    else
    {
        if (rdCommon->HGET(strKey.c_str() , strField.c_str() , strRet) && !strRet.empty())
        {
            Json::Reader jReader;
            Json::Value jsonconf;
            jReader.parse(strRet , jsonconf);
            out["phoneRegister"] = Json::SafeConverToInt32(jsonconf["phoneRegister"]);
            out["paytype"] = Json::SafeConverToInt32(jsonconf["paytype"] , Json::SafeConverToInt32(out["paytype"]));
            out["pmode"] = Json::SafeConverToInt32(jsonconf["pmode"] , Json::SafeConverToInt32(out["pmode"]));
            out["othertype"] = Json::SafeConverToInt32(jsonconf["othertype"] , Json::SafeConverToInt32(out["othertype"]));
            out["otherpay"] = Json::SafeConverToInt32(jsonconf["otherpay"] , Json::SafeConverToInt32(out["otherpay"]));
            out["bank"] = Json::SafeConverToInt32(jsonconf["bank"] , Json::SafeConverToInt32(out["bank"]));

            LOGGER(E_LOG_INFO) << "read common redis return is[ip:" << rdCommon->GetIp() << ",port:" << rdCommon->GetPort()
                << "]; msg : " << strRet << "; HGET Key Is : " << strKey << "; Field Is :" << strField
                << ";phoneRegister: " << out["phoneRegister"].asString() << ";otherpay: " << out["otherpay"].asString();
        }
        else
        {
            LOGGER(E_LOG_INFO) << "read common redis return false[ip:" << rdCommon->GetIp() << ",port:" << rdCommon->GetPort() << "]";
        }

        //新增支付方式二维码扫描下载
        std::string strSetting1 , strSetting2;
        Json::Value jvSetting1 , jvSetting2;
        Json::Reader jrTmp;
        if (rdCommon->HGET("RechargeQrcodeSetting|1" , "1" , strSetting1)) jrTmp.parse(strSetting1 , jvSetting1);
        if (rdCommon->HGET("RechargeQrcodeSetting|2" , "1" , strSetting2)) jrTmp.parse(strSetting2 , jvSetting2);
        out["qrcodeinfo"][0] = jvSetting1;
        out["qrcodeinfo"][1] = jvSetting2;
    }

    vector<string> ip_arr = Lib_Ip::find(client_ip);
    if(ip_arr.size() > 0)
	{
		LOGGER(E_LOG_INFO) << client_ip << " you country is:" << ip_arr[0];
		/*
		if (ip_arr[0] == "中国" || ip_arr[0] == "局域网" || ip_arr[0] == "柬埔寨")
		{
			is_china_ip = 1;
		}*/

		int is_china_ip     = 0;
		std::string strField = "xzip";
		std::string strRet;
		if(rdCommon->HGET(strKey.c_str(), strField.c_str(), strRet) && !strRet.empty())
		{
			vector<string> vsCountries = Helper::explode(strRet.c_str(), ",");
			for (int i = 0 ; i < vsCountries.size(); i++)
			{
				if (vsCountries[i] == ip_arr[0])
				{
					is_china_ip = 1;
					break;
				}
			}
		}
		if (strRet.empty())
		{
			is_china_ip = 1;
		}

		if (is_china_ip == 0) 
		{
			LOGGER(E_LOG_INFO) << client_ip << " is_china_ip == 0";
			out["otherpay"] = 0;
			out["pmode"] = 8;
		}
	}

    LOGGER(E_LOG_INFO) << "pmode:" << out["pmode"].asString() << " ;otherpay: " << out["otherpay"].asString()<<" othertype:"<<out["othertype"].asString();

//    // 安卓默认汇支付
//    if (ctype == 1) 
//	{
//        out["otherpay"] = 2;
//    }

    return status_ok;
}
