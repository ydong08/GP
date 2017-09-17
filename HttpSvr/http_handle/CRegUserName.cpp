#include <set>
#include <algorithm>
#include "CRegUserName.h"
#include "json/json.h"
#include "Logger.h"
#include "Helper.h"
#include "threadres.h"
#include "Lib_Ip.h"
#include "Util.h"
#include "md5.h"
#include "RedisLogic.h"
#include "SqlLogic.h"

int CRegUserName::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    Json::Value data;
	Json::FastWriter jWriter;
	out["result"]       = 0;
	if (root["sid"].isNull() || root["cid"].isNull() || root["ctype"].isNull() || root["pid"].isNull() || root["versions"].isNull() || 
            root["param"].isNull() || root["param"]["password"].isNull() || 
            (root["os_version"].isNull() && root["param"]["os_version"].isNull()) || 
            (root["device_name"].isNull() && root["param"]["device_name"].isNull()) )
	{
		out["msg"]       = "参数不正确";
		//out = jWriter.write(ret);
		return status_ok;
	}

    RedisAccess* common = RedisLogic::GetRdCommon();
	if (NULL == common)
	{
		out["msg"]       = "获取缓存管理中心信息出错";
		//out = jWriter.write(ret);
		return status_user_error;
	}

	data["sid"]         = Json::SafeConverToInt32(root["sid"]);//账号类型ID
	data["cid"]         = Json::SafeConverToInt32(root["cid"]);//渠道ID
	data["ctype"]       = Json::SafeConverToInt32(root["ctype"]);//客户端类型 	1 android 2 iphone 3 ipad 4 android pad
	data["pid"]         = Json::SafeConverToInt32(root["pid"]);//客户端包类型ID
	data["versions"]    = Json::SafeConverToInt32(root["versions"]);
    data["ip"]          = client_ip;

    data["device_name"] = root["param"]["device_name"].asString();//设备名称
    data["os_version"]  = root["param"]["os_version"].asString();//操作系统版本号
    data["net_type"]    = Json::SafeConverToInt32(root["param"]["net_type"]);//网络设备类型

    data["username"]    = Helper::filterInput(Helper::strtolower(root["param"]["username"].asString().c_str()));
    data["password"]    = Helper::trim(root["param"]["password"].asString().c_str());
    //data["mnick"]       = param["param"]["username"] ? Helper::filterInput(strtolower(param["param"]["username"])) : "";
    //data["mnick"]       = param["param"]["mnick"] ? Helper::filterInput(param["param"]["mnick"]) : "player".rand(1, 10000);
    redisReply * reply = common->CommandV("spop defaultPlayname");
    if (reply && reply->type == REDIS_REPLY_STRING)
        data["mnick"] = reply->str;
    else
        data["mnick"] = "";
    freeReplyObject(reply);
    data["mnick"] = data["mnick"].asString().empty() ? data["username"] : data["mnick"];
    data["gameid"]      = Json::SafeConverToInt32(root["gameid"], 1);
    data["productid"]   = Json::SafeConverToInt32(root["productid"], 1);//产品ID
	const char* openudid = root["param"]["openudid"].asString().c_str();
    const char* captcha_ = root["param"]["captcha"].asString().c_str();

    int is_USA_ip = 0;
    vector<string> ip_arr = Lib_Ip::find(client_ip);
    if(ip_arr.size() > 0 && ip_arr[0] == "美国")
	{
        is_USA_ip = 1;
        data["mnick"] = StrFormatA("player%d", rand()%10000);
    }
    if (ip_arr.size() > 2 && strstr(ip_arr[1].c_str(),"辽宁") && strstr(ip_arr[2].c_str(),"阜新")) 
	{
        //out = jWriter.write(ret);
        return status_ok;
    }

    string ip = client_ip;
    string device_no   = getDeviceNo(root, ip);
    string svr_captcha;
    reply = common->CommandV(StrFormatA("get CPA|%s", device_no.c_str()).c_str());
    if (reply && reply->type == REDIS_REPLY_STRING)
        svr_captcha = reply->str;
    freeReplyObject(reply);

    string captcha = Helper::strtolower(captcha_);
    if(captcha != svr_captcha)
	{
        LOGGER(E_LOG_DEBUG) << "param=" << jWriter.write(root) << " svr=" << svr_captcha << " device_no=" << device_no << "captchas_errer";
        out["msg"]    = "验证码错误";
        out["result"] = -10;
        //out = jWriter.write(ret);
        return status_ok;
    }

    if(data["username"].asString().empty() || data["password"].asString().empty()){//用户名或密码为空
        out["msg"]    = "用户名或密码为空";
        out["result"] = -2;
        //out = jWriter.write(ret);
        return status_ok;
    }

    if(!Helper::isUsername(data["username"].asString().c_str())){//用户名不合法
        out["msg"]    = "用户名格式错误";
        out["result"] = -2;
        //out = jWriter.write(ret);
        return status_ok;
    }

    //unimplement
//    string username = Loader_Tcp::callServer2CheckName()->checkUserName(data["username"]);//调C++词库看用户名是否合法
    string username = data["username"].asString();
    if(strstr(username.c_str(), "*")){
        out["msg"]    = "用户名格式错误";
        out["result"] = -2;
        //out = jWriter.write(ret);
        return status_ok;
    }

    string limitKey = getDeviceNo(root, ip);
    data["device_no"] = limitKey;

    if(limitKey.size() > 0){
        int count = RedisLogic::limitCount(limitKey, "reg", 1,false, 24*3600);

        if(is_USA_ip != 1){//针对非美国IP
            if(count > 5){//单个设备限制
                out["msg"]    = "注册次数超过最大注册数";
                out["result"] = -6;
                //out = jWriter.write(ret);
                return status_ok;
            }

            count = RedisLogic::limitCount(ip, "ip_reg", 1,false, 24*3600);
            if(count > 50){
                LOGGER(E_LOG_DEBUG) << ip << "|" << data["username"].asString() << "|" << data["device_name"].asString() << "|" << data["gameid"].asString() << "|" << 
					data["cid"].asString() << "ip_register_deny";
                out["msg"]    = "注册次数超过最大注册数";
                out["result"] = -6;
                //jWriter.write(ret);
                return status_ok;
            }
        }
    }

//    root["ip"] = ip.c_str();

    RedisAccess * account = RedisLogic::GetRdAccount(); 
    reply = account->CommandV("sadd USN %s", data["username"].asString().c_str());
    int username_flag = 1;
    if (reply && reply->type == REDIS_REPLY_INTEGER)
        username_flag = reply->integer;
    freeReplyObject(reply);
    if(username_flag == 0){
        out["msg"]    = "用户名已存在";
        out["result"] = -5;
        //jWriter.write(ret);
        return status_ok;
    }
	string strUserName = data["username"].asString();
    if(isMobile(strUserName))
	{
        int phone_flag = 1;
        reply = account->CommandV("sadd PHN %s", data["username"].asString().c_str());
        if (reply && reply->type == REDIS_REPLY_INTEGER)
            phone_flag = reply->integer;
        freeReplyObject(reply);
        if(phone_flag == 0){
            out["msg"]    = "该手机号已注册";
            out["result"] = -6;
            //jWriter.write(ret);
            return status_ok;
        }
    }

	data["country"] = root["param"]["country"];//国家/64个字符
	data["province"] = root["param"]["province"];//省份/64个字符
	data["city"] = root["param"]["city"];		//城市/64个字符
	data["equipment_model"] = root["param"]["equipment_model"];	//手机设备型号/128个字符
	data["networking"] = root["param"]["networking"];//联网方式/10个字符  3G 4G等等
	data["sex"] = root["param"]["sex"];

    bool result= SqlLogic::registerByUserName(data) != -1;
    out["result"] = result ? 1 : 0;

    if (result) {
        RedisLogic::limitCount(limitKey, "reg", 1,true, 24*3600);
        RedisLogic::limitCount(ip, "ip_reg", 1,true, 24*3600);
    }

    return status_ok;
}
