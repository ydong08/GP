#include "CGuestLogin.h"
#include "json/json.h"
#include "Logger.h"
#include <string>

#include "MySqlDBAccess.h"
#include "../threadres.h"
#include "JsonValueConvert.h"
#include "Helper.h"
#include "RedisLogic.h"
#include "SqlLogic.h"
#include "CConfig.h"

/*******************************************************************************************
#define HTTP_OK 200

#define REQ_SUCCESS 200
#define REQ_NO_RIGHT 100
#define REQ_PARAM_ERROR 101
#define REQ_PARAM_LACK 102
#define REQ_METHOD_ERROR 103
#define REQ_REGISTER_ERROR 104
#define REQ_GETINFO_ERROR 105
#define REQ_ACCOUNT_LIMIT 106//被封号
#define REQ_UPLOAD_FORMAT_ERROR 107
#define REQ_USER_PWD_ERROR 108

游客登录参数
{
	"api": 2,
	"cid" : 3,
	"ctype" : 1,
	"gameid" : 7,
	"mid" : 0,
	"mtkey" : "",
	"pid" : 1145,
	"productid" : 1145,
	"sid" : 100,
	"sig" : "5b9665a1139d37a18c553e0dc8ff982a",
	"versions" : 10,
	"param" : {
		"deviceToken": "",
		"device_name" : "MINOTELTE",
		"device_no" : "867993025566066",
		"method" : "Login.guest",
		"mnick" : "MI NOTE LTE",
		"mobileOperator" : 0,
		"net_type" : 2,
		"os_version" : "6.0.1",
		"uuid" : "39b47bca-b372-4852-a172-a41df0f14588"
	}
}

{
	"flag": 200
	"data":
		{
			"pid":
			"reg_pid":
			"versions":
			"cid":
			"pid":
			"ctype":
			"lang":
			"devicename":
			"osversion":
			"nettype":
			"device_no":
			"gameid":
			"productid":
			"deviceToken":
			"mtype":
			"result" : 1,
			"mnick":
			"username":
		}
}

*******************************************************************************************/

bool CGuestLogin::IsForbidLogin(std::string& ip, std::string& device_no, std::string& openudid)
{
	// 到mysql 中查找
	//MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
	if (SqlLogic::GetDeviceForbidInfo(ip, device_no,openudid))
	{
		_LOG_INFO_("Mysql fit in!!! This device forbid to login game !");
		return true;
	}

	return false;
}

bool CGuestLogin::IsForbidLoginByMid(int32_t Mid)
{
	if (SqlLogic::GetDeviceForbidInfoByMid(Mid))
	{
		_LOG_INFO_("Mysql fit in!!! This device forbid to login game !");
		return true;
	}
	return false;
}

int CGuestLogin::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	out["result"] = 0;
	if (!RedisLogic::MaintenanceCheck(client_ip))
	{
		out["msg"] = "系统维护中，请稍候再试...";
		return status_faild_access;
	}

	if (root["mid"].isNull() || root["cid"].isNull() || root["ctype"].isNull() || root["pid"].isNull() || root["versions"].isNull() || root["productid"].isNull())
	{
		out["msg"] = "参数不正确";
		return status_register_error;
	}

	uint32_t nSid	  = 100; //账号类型ID	
	uint32_t uCtype     = Json::SafeConverToUInt32(root["ctype"]);	//客户端类型 	1 android 2 iphone 3 ipad 4 android pad
	int nGameId    = Json::SafeConverToInt32(root["gameid"]);	

	Json::Value jsData;
	jsData["sid"] = nSid; //账号类型ID	
	jsData["cid"] = Json::SafeConverToUInt32(root["cid"]); //渠道ID
	jsData["ctype"] = uCtype; //客户端类型 	1 android 2 iphone 3 ipad 4 android pad	
	jsData["pid"] = Json::SafeConverToUInt32(root["pid"]); //客户端包类型ID
	jsData["versions"] = root["versions"];
	jsData["ip"] = client_ip;
	jsData["lang"] = Json::SafeConverToUInt32(root["lang"]); //设备语言

	jsData["mnick"] = (!root["param"]["mnick"].asString().empty()) ? Helper::filterInput(root["param"]["mnick"].asString()) : StrFormatA("player%d",GetRandInt(1,10000)) ;

	jsData["device_name"] = root["param"]["device_name"]; //设备名称

    if ( CConfig::getInstance().m_SimulatorAllowed == 1 && 
            ( root["param"]["equipment_model"].asString() == "iPhone Simulator" || 
              root["param"]["equipment_model"].asString() == "iPad Simulator" ) )
    {
        out["msg"] = "...";
        return status_faild_access;
    }
	jsData["os_version"] = root["param"]["os_version"]; //操作系统版本号
	string strNetType = TrimStr(root["param"]["net_type"].asString());
	if (!strNetType.empty()) ToLower(strNetType);
	jsData["net_type"] = strNetType; //网络设备类型
//	jsData["device_no"] = root["param"]["device_no"].asString().empty() ? jsData["ip"] : root["param"]["device_no"]; //android 机器码
	jsData["gameid"] = Json::SafeConverToUInt32(root["gameid"],1); //游戏ID
	uint32_t nProductId = Json::SafeConverToUInt32(root["productid"],1);
	jsData["productid"] = nProductId; //产品ID
	jsData["deviceToken"] = Json::SafeConverToStr(root["param"]["deviceToken"],"0"); //推送的机器码

	std::string strOpenUUID  = TrimStr(root["param"]["openudid"].asString());
	std::string strClientIp = client_ip;
	std::string strDeviceNo = getDeviceNo(root,strClientIp);
	jsData["device_no"] = strDeviceNo;
	if (IsForbidLogin(strClientIp, strDeviceNo, strOpenUUID))
	{
		out["msg"] = "该用户已经被限制登录了";
		return status_faild_access;
	}


	int nSitemId = (uCtype == 1) ? SqlLogic::GetSiteMidByKey(strDeviceNo, nProductId) : SqlLogic::getSitemidByIosKey(strDeviceNo,strOpenUUID,nProductId);
	if (nSitemId <= 0)
	{
		out["msg"] = "获取sitemid信息出错";
		return status_register_error;
	}

	std::string strUserName;
	std::string strNickName;
	std::string strPassword;
	//Json::Value jvUserRet;
	if (0 != SqlLogic::getOneBySitemid(nSitemId , nSid , out , true))
	{
		//注册
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (NULL == redisMgr)
		{
			out["msg"] = "获取玩家缓存信息出错";
			return status_register_error;
		}
		RedisAccess* account = redisMgr->account("slave");
		if (NULL == account)
		{
			out["msg"] = "获取玩家缓存信息出错";
			return status_register_error;
		}

		std::string strDeviceName = root["device_name"].asString();
		size_t nPos = strDeviceName.find("Lan7");
		
		std::string strKey = StrFormatA("BAT|%s",strDeviceNo.c_str());
		std::string strDeviceFlag;
		bool bDeviceFlag = account->GET(strKey.c_str(),strDeviceFlag);
		strKey = StrFormatA("BAT|%s",client_ip);
		std::string strIpFlag;
		bool bIpFlag = account->GET(strKey.c_str(),strIpFlag);

		if (nPos != std::string::npos || bDeviceFlag || bIpFlag) 
		{
			LOGGER(E_LOG_WARNING)<< "flag:" << nPos<< "|deviceFlag:" << bDeviceFlag<< "|strDeviceFlag:" << strDeviceFlag<< "|bIpFlag:" << bIpFlag<< "|strIpFlag:" << strIpFlag<< "|ban_deny_guest" ;
			out["msg"] = "游客注册出错";
			return status_register_error;
		}
		int64_t nCount;
		std::string strUUID = TrimStr(root["param"]["uuid"].asString());
		if (!strUUID.empty())
		{
			nCount = SqlLogic::limitCount(strUUID, "uuid", 1, true, 24 * 3600);
			if (nCount > 5)	//uuid防刷
			{
				LOGGER(E_LOG_WARNING)<< "device_no:" << strDeviceNo<< "|device_name:" << strDeviceName<< "|gameid:" << nGameId<< "|uuid:" << strUUID<< "|uuid_guest_deny" ;
				out["msg"] = "同一设备次数超过5次了";
				return status_register_error;
			}
		}
		nCount = SqlLogic::limitCount(strClientIp, "ip_reg", 1, true, 24 * 3600);
		if (nCount > 50 && strClientIp != "58.210.18.172" && strClientIp.find("192.168.1") != 0) 
		{//IP防刷
			LOGGER(E_LOG_WARNING)<< "device_no:" << strDeviceNo<< "|device_name:" << strDeviceName<< "|gameid:" << nGameId<< "|uuid:" << strUUID<< "|uuid_guest_deny" ;
			out["msg"] = "同一IP次数超过50次了";
			return status_register_error;
		}
		jsData["sitemid"] = nSitemId;
		account = redisMgr->account();
		if (NULL == account)
		{
			out["msg"] = "获取玩家的缓存信息出错";
			return status_register_error;
		}
		while (true)
		{
			strUserName = GetRandStringOnly(10);
			if(account->SADD("USN",strUserName.c_str())) break;
		}
		jsData["username"] = strUserName;
		strNickName = account->SPOP("defaultPlayname");
		if(strNickName.empty()) strNickName = strUserName;
		jsData["mnick"] = strNickName;
		jsData["password"] = GetRandStringOnly(10);
		
		jsData["country"] = root["param"]["country"];//国家/64个字符
		jsData["province"] = root["param"]["province"];//省份/64个字符
		jsData["city"] = root["param"]["city"];		//城市/64个字符
		jsData["equipment_model"] = root["param"]["equipment_model"];	//手机设备型号/128个字符
		jsData["networking"] = root["param"]["networking"];//联网方式/10个字符  3G 4G等等
		jsData["sex"] = root["param"]["sex"];
		jsData["versions"] = root["versions"];

		if(-1 == SqlLogic::registerByUserName(jsData,out))	//这个函数里面有执行过getOneById,下面的函数，应该算用重了
		{
			out["msg"] = "用户注册失败";
			return status_register_error;

		}
		nSitemId = Json::SafeConverToInt32(out["sitemid"]);
	}
	
	out["reg_pid"] = out["pid"]; //注册时的pid
	out["versions"] = jsData["versions"];
	out["cid"] = jsData["cid"];
	out["pid"] = jsData["pid"];
	out["ctype"] = jsData["ctype"];
	out["lang"] = jsData["lang"]; //设备语言
	out["devicename"] = jsData["device_name"];
	out["osversion"] = jsData["os_version"]; //操作系统版本号
	out["nettype"] = jsData["net_type"]; //网络设备类型
	out["device_no"] = strDeviceNo; //机器码
	out["ip"] = client_ip;
	out["gameid"] = jsData["gameid"];
	out["productid"] = jsData["productid"];
	out["deviceToken"] = jsData["deviceToken"];
	out["mtype"] = Json::SafeConverToUInt32(root["param"]["mobileOperator"]); //1 移动卡  2 联通卡  3 电信卡
	strUserName = "";
	if (!SqlLogic::getUserNameBySitemid(nSitemId, Json::SafeConverToInt32(out["sid"]),strUserName))
	{
		out["msg"] = "获取用户名称失败";
		return status_register_error;
	}
	out["usen"] = strUserName;
	if (out["usen"] == out["mnick"])
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (NULL == redisMgr)
		{
			out["msg"] = "获取玩家缓存信息出错";
			return status_register_error;
		}
		RedisAccess* redisCommon = redisMgr->common();
		if (redisCommon)
		{
			jsData["mnick"] = redisCommon->SPOP("defaultPlayname");
		}
		strUserName = Json::SafeConverToStr(jsData["mnick"]);
		if (strUserName.empty()) strUserName = Json::SafeConverToStr(jsData["username"]);
		if (strUserName.empty()) strUserName = GetRandStringOnly(10);
		out["mnick"] = strUserName;
		RedisLogic::SetUserInfo(Json::SafeConverToInt32(out["mid"]), out, 0);
	}
	//这里等mid生成了 在限制一次
	/*if (IsForbidLogin(strClientIp, strDeviceNo, Json::SafeConverToInt32(out["mid"])))
	{
	out["msg"] = "该用户已经被限制登录了";
	return status_faild_access;
	}*/


	if (IsForbidLoginByMid(Json::SafeConverToInt32(out["mid"])))
	{
		out["money"] = 0;
		out["serverInfo"]["hallAddr"] = 0;
		out["serverInfo"]["hallAddrBackup"] = 0;
		out["msg"] = "该用户ID已经被限制登录了";
		return status_faild_access;
	}


	out["mnick"] = Helper::filterInput(out["mnick"].asString());
	strUserName = Json::SafeConverToStr(out["mnick"]);
	if (strUserName.empty()) out["mnick"] = GetRandStringOnly(10);
	out["username"] = "游客";

    if (RedisLogic::doUserInfo(out) != 0)
    {
        out["msg"] = "获取玩家信息出错";
        return status_register_error;
    }
	out["result"] = 1;
//	out["iconid"] = 0;
//	out["binding"] = 0;
    
    RedisLogic::getServerInfo(nGameId, nProductId, out);
	RedisLogic::getYuleInfo(out);
	RedisLogic::getOfficialWechat(out);
	RedisLogic::getUserRoomCardInfo(out);
	
    LOGGER(E_LOG_DEBUG) << out.toStyledString();
	
	return status_ok;
}


