#include "CIssuedAccount.h"
#include "json/json.h"
#include "Logger.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <regex>

#include "../threadres.h"
#include "MySqlDBAccess.h"
#include "md5.h"
#include "SqlLogic.h"

using namespace std;

int CIssuedAccount::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	Json::FastWriter writer;
	//Json::Value info;
	out["result"] = 0;
	out["username"] = "";
	out["password"] = "";
	if (root["sid"].isNull() || root["cid"].isNull() || root["ctype"].isNull() || root["pid"].isNull() || root["versions"].isNull() || 
		root["mid"].isNull() || root["param"].isNull() || root["gameid"].isNull())
	{
		out["msg"] = "参数不正确";
		//out = writer.write(info);
		return status_argment_access;
	}

    int sid = Json::SafeConverToInt32(root["sid"]); //账号类型ID
    int cid = Json::SafeConverToInt32(root["cid"]); //渠道ID
    int ctype = Json::SafeConverToInt32(root["ctype"]); //客户端类型 	1 android 2 iphone 3 ipad 4 android pad
    int pid = Json::SafeConverToInt32(root["pid"]); //客户端包类型
    std::string versions = root["versions"].asString();
    int uid = Json::SafeConverToInt32(root["mid"]);
    std::string deviceno = root["param"]["device_no"].asString();
    if (deviceno.empty())
    {
        deviceno = client_ip;
    }
    int gameid = Json::SafeConverToInt32(root["gameid"],1);

    std::string osversion = root["param"]["os_version"].asString();
    std::string advertising = root["param"]["advertising"].asString();
    std::string mac_address = root["param"]["mac_address"].asString();
    
    if (cid == 0 || ctype == 0 || pid == 0)
    {
        out["msg"] = "参数不正确";
        return status_ok;
    }
 //   MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
    std::string username;
    std::string pwd = StrFormatA("%d", GetRandInt(100000, 999999));
    int sitemid = 0;
    for (int i = 0; i < 20; i++)
    {
        username = GetRandString(GetRandInt(7, 11));
        sitemid = SqlLogic::getSitemidByName(username, pwd);
        if (sitemid == 0)   break; //说明数据库不存在该账号

        username = "";
    }
    if (username.empty())
    {
		out["msg"] = "参数不正确";
		return status_ok;
    }
    MD5 m5(pwd);
    std::string strPwdMd5 = m5.md5();
    sitemid = SqlLogic::addRegisterUserName(username, strPwdMd5);
    if (sitemid == 0)
    {
		out["msg"] = "玩家注册失败";
		return status_ok;
    }
    out["result"] = 1;
    out["username"] = username;
    out["password"] = pwd;
 //   info["mid"] = sitemid;
    //out = writer.write(info);
    return status_ok;
}
