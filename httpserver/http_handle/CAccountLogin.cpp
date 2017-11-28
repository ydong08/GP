#include "CAccountLogin.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "MySqlDBAccess.h"
#include "JsonValueConvert.h"
#include "RedisLogic.h"
#include "SqlLogic.h"
#include "CConfig.h"

/******************************************************************************************************
{
	"api": 1,
	"cid": 141,
	"ctype": 2,
	"gameid": 7,
	"mid": 0,
	"mtkey": "",
	"param": {
		"advertising": "1CB1E9CA-CDC2-4B3C-A3E6-C684075EC946",
		"deviceToken": "",
		"device_name": "孙海波的MacBookPro",
		"device_no": "",
		"mac_address": "C4:B3:01:B9:7F35",
		"method": "Login.account",
		"mnick": "",
		"mobileOperator": 0,
		"net_type": 2,
		"openudid": "b1ff205680020089d6299216e4b697f8690f4f38",
		"os_version": "10.0",
		"password": "qwerty",
		"phoneno": "",
		"sex"1,
		"sitemid": "",
		"siteurl": "",
		"username": "Mooo",
		"uuid": "",
		"vendor": "1776D80A-11D7-4C20-9568-AD7C29469E3D"
	},
	"pid": 11,
	"productid": 11,
	"sid": 102,
	"sig": "15bd420a6c0fae2b6977e4e8f4ff2584",
	"versions": 2
}
******************************************************************************************************/

bool CAccountLogin::IsForbidLogin(std::string& ip, std::string& device_no, std::string& openudid)
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

bool CAccountLogin::IsForbidLoginByMid(int32_t Mid)
{
	if (SqlLogic::GetDeviceForbidInfoByMid(Mid))
	{
		_LOG_INFO_("Mysql fit in!!! This device forbid to login game !");
		return true;
	}
	return false;
}

int CAccountLogin::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	if (!RedisLogic::MaintenanceCheck(client_ip))
	{
		out["msg"] = "系统维护中，请稍候再试...";
		return status_faild_access;
	}
 //   int api = Json::SafeConverToInt32(root["api"]);
    int cid = Json::SafeConverToInt32(root["cid"]);
    int gameid = Json::SafeConverToInt32(root["gameid"]);
    int mid = Json::SafeConverToInt32(root["mid"]);

	/*if (IsForbidLoginByMid(mid))
	{
	out["msg"] = "该用户ID已经被限制登录了";
	return status_faild_access;
	}*/

    int ctype = Json::SafeConverToInt32(root["ctype"]);
//    std::string mtkey = root["mtkey"].asString();
    int pid = Json::SafeConverToInt32(root["pid"]);
    int productid = Json::SafeConverToInt32(root["productid"]);
    int sid = Json::SafeConverToInt32(root["sid"]);
//    std::string  sig = root["sig"].asString();
    int versions = Json::SafeConverToInt32(root["versions"]);
	
//	std::string advertising = root["param"]["advertising"].asString();
	std::string deviceToken = root["param"]["deviceToken"].asString();
	std::string device_name = root["param"]["device_name"].asString();

    if ( CConfig::getInstance().m_SimulatorAllowed == 1 && 
            ( root["param"]["equipment_model"].asString() == "iPhone Simulator" || 
              root["param"]["equipment_model"].asString() == "iPad Simulator" ) )
    {
        out["msg"] = "...";
        return status_faild_access;
    }

	//std::string device_no = root["param"]["device_no"].asString();
//	std::string mac_address = root["param"]["mac_address"].asString();
//	std::string mnick = root["param"]["mnick"].asString();
	int mobileOperator = Json::SafeConverToInt32(root["param"]["mobileOperator"]);
	int net_type = Json::SafeConverToInt32(root["param"]["net_type"]);
	std::string openudid = root["param"]["openudid"].asString();
	std::string os_version = root["param"]["os_version"].asString();
	std::string password = root["param"]["password"].asString();
	std::string phoneno = root["param"]["phoneno"].asString();
//	int sex = Json::SafeConverToInt32(root["param"]["sex"]);
	//std::string sitemid = root["param"]["sitemid"].asString();
//	std::string siteurl = root["param"]["siteurl"].asString();
	std::string strUserName = root["param"]["username"].asString();
//	std::string vendor = root["param"]["vendor"].asString();
//	std::string uuid = root["param"]["uuid"].asString();
//	std::string method = root["param"]["method"].asString();
	/*
    _LOG_DEBUG_("api:%d, cid:%d, gameid:%d, mid:%d, mtkey:%s, pid:%d, productid:%d, sid:%d, sig:%s, versions:%d, method:%s ",
        api, cid, gameid, mid, mtkey.c_str(), pid,productid,sid,sig.c_str(),versions, method.c_str());
		*/
    //Json::Value user;

    //Json::Value ret;
    std::string ip = client_ip;
    std::string dev_no = getDeviceNo(root, ip);
    
     _LOG_DEBUG_("Check is forbid log in!");

    if (IsForbidLogin(ip , dev_no , openudid))
    {
        ULOGGER(E_LOG_INFO,mid) << ip << " , " << dev_no << " ," << openudid << " is forbid to login";
        out["msg"] = "玩家被禁止登录";
        return status_faild_access;
    }

     _LOG_DEBUG_("get sitemid ...!");

    int sitemid = 0;
	bool bMobileUserName = isMobile(strUserName);
    if ( bMobileUserName )
    {
        sid = 101;
        sitemid = SqlLogic::getSitemidByPhoneNumber(strUserName , password);
		
        if ( sitemid == 0 )
        {
            sitemid = SqlLogic::getAccountByBinding(strUserName , password, productid);
            sid = 102;
        }
    }
	if (0 == sitemid)
    {
		switch (sid)
		{
		case 101:
			{
				if (!bMobileUserName)	//mobile已经在上面测过了
				{
					sitemid = SqlLogic::getSitemidByPhoneNumber(phoneno , password);
					strUserName = phoneno;
				}
			}
			break;
		case 102://普通账号
			sitemid = SqlLogic::getSitemidByuserName(strUserName , password);
			break;
		default:
			break;
		}
    }

	if (sid != 120 && 0 == sitemid)
	{
		return status_account_error;
	}
    
    if (0 == sitemid)
    {
        _LOG_DEBUG_("Failed to get sitemid!");
        out["msg"] = "无法从数据库获取玩家平台mid";
        return status_user_error;
    }

    _LOG_DEBUG_("get One By Sitemid ...!");
    
    if (0 != SqlLogic::getOneBySitemid(sitemid , sid , out , true))
	{
        _LOG_DEBUG_("Failed to get use info by sitemid!");
        out["msg"] = "无法通过mid获取玩家信息";
        return status_user_error;
    }
    
    _LOG_DEBUG_("check product id ...!");

    if (Json::SafeConverToInt32(out["productid"]) != productid)
	{
		RedisLogic::unsetUserFiled(out);
        ULOGGER(E_LOG_INFO , mid) << ip << " , " << dev_no << " ," << openudid << " product id is invalid !";
        out["msg"] = "玩家产品信息错误";
        return status_user_error;
    }

    _LOG_DEBUG_("init user by param ...!");
    
    out["device_no"] = dev_no; //机器码
    out["gameid"] = gameid;
    out["reg_pid"] = pid; //注册时的pid
    out["versions"] = versions;
    out["cid"] = cid;
    out["pid"] = pid;
    out["ctype"] = ctype;
    out["devicename"] = device_name;
    out["osversion"] = os_version;//操作系统版本号
    out["nettype"] = net_type;//网络设备类型
    out["ip"] = ip;
    out["deviceToken"] = deviceToken;
    out["mtype"] = mobileOperator;//1 移动卡  2 联通卡  3 电信卡
	out["result"] = 1;
	
	out["productid"] = productid;

	if (out["mnick"].asString().empty()) out["mnick"] = GetRandStringOnly(10);
	out["username"] = strUserName;

    _LOG_DEBUG_("get redis mgr ...!");
    
/*    RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
    if (redisMgr == NULL)
    {
        out["msg"] = "访问redis出错";
        ULOGGER(E_LOG_INFO , mid) << "访问redis出错";

        return -1;
    }*/

    _LOG_DEBUG_("do User Info ...!");
    
	out["country"] = root["param"]["country"];//国家/64个字符
	out["province"] = root["param"]["province"];//省份/64个字符
	out["city"] = root["param"]["city"];		//城市/64个字符
	out["equipment_model"] = root["param"]["equipment_model"];	//手机设备型号/128个字符
	out["networking"] = root["param"]["networking"];//联网方式/10个字符  3G 4G等等

	if (RedisLogic::doUserInfo(out) != 0)
	{
		RedisLogic::unsetUserFiled(out);
		out["msg"] = "获取用户信息失败";
        ULOGGER(E_LOG_INFO , mid) << "doUserInfo 访问redis出错";
        return status_user_error;
    }
	uint32_t myMid = Json::SafeConverToInt32(out["mid"]);
	if (IsForbidLoginByMid(myMid))
	{
		out["money"] = 0;
		out["serverInfo"]["hallAddr"] = 0;
		out["serverInfo"]["hallAddrBackup"] = 0;
		out["msg"] = "该用户ID已经被限制登录了";
		return status_faild_access;
	}




	RedisLogic::getOfficialWechat(out);
	RedisLogic::getUserRoomCardInfo(out);

    LOGGER(E_LOG_DEBUG) << "user info is:" << out.toStyledString();
    return status_ok;
}

