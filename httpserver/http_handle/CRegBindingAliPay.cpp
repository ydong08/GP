#include "CRegBindingAliPay.h"
#include "json/json.h"
#include "Logger.h"
//#include "GameInfo.h"
#include "RedisAccess.h"
#include "../threadres.h"

//#include "../comm/GameInfo.h"

#include "RedisLogic.h"
#include "SqlLogic.h"

#include <regex.h>

/***********************************************************************************
// 接口参数
{
	"api":2,
	"cid":3,
	"ctype":1,
	"gameid":7,
	"mid":6877738,
	"mtkey":"6c7a46d80db89a50049dfcff54da0e0c",
	"param":{
		"aliname":"广义",
		"alipay":"hhhhhh@163.com",
		"gameid":7,
		"method":"Register.bindingAlipay"
	},
	"pid":1145,
	"productid":1145,
	"sid":100,
	"sig":"14176e003a6a504b7ab306b0f9254966",
	"versions":10
}
************************************************************************************/

int CRegBindingAliPay::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	int gameid = Json::SafeConverToInt32(root["gameid"]);
	int mid = Json::SafeConverToInt32(root["mid"]);
	std::string aliname = root["param"]["aliname"].asString();
	std::string alipay = root["param"]["alipay"].asString();

	aliname = TrimStr(aliname);
	alipay = TrimStr(alipay);
    if (mid <= 0 || gameid <= 0 || aliname.empty() || alipay.empty())
    {
		out["result"] = -1;
		out["msg"] = "缺少参数";
        return status_ok;
    }
	CAutoUserDoing userDoing(mid);
	if (userDoing.IsDoing())
	{
		out["result"] = -1;
		out["msg"] = "您操作太快了，请休息一会！";
		return status_ok;
	}
    if (!isEmail(alipay) && !isMobile(alipay) )
    {
		out["result"] = -1;
		out["msg"] = "支付宝账号不正确";
        return status_ok;
	}
	Json::Value jvUserInfo;
	if (!SqlLogic::GetUserInfo(mid,jvUserInfo))
	{
		out["result"] = -1;
		out["msg"] = "没有该用户的信息";
		return status_ok;
	}

	Json::Value jvAliInfo;
	jvAliInfo["mid"] = mid;
	jvAliInfo["alipay"] = alipay;
	jvAliInfo["aliname"] = aliname;
	jvAliInfo["gameid"] = gameid;
	bool bBindOk = SqlLogic::bindingAlipay(jvAliInfo);
	
	if (bBindOk)
	{
		// 提交时间，内容绑定支付宝(姓名：支付宝号：)成功  状态已处理
		std::string strContent = StrFormatA("绑定支付宝(姓名：%s 支付宝号：%s)成功",aliname.c_str(),alipay.c_str());
		SqlLogic::feedBack(gameid
			,Json::SafeConverToInt32(jvUserInfo["cid"])
			,Json::SafeConverToInt32(jvUserInfo["sid"])
			,Json::SafeConverToInt32(jvUserInfo["pid"])
			,Json::SafeConverToInt32(jvUserInfo["ctype"])
			,strContent.c_str()
			,Json::SafeConverToInt32(jvUserInfo["mid"])
			,Json::SafeConverToStr(jvUserInfo["mnick"]).c_str()
			,""
			,""
			,client_ip
			,3
			,Json::SafeConverToInt32(jvUserInfo["productid"])
			);
	}
	else
	{
		out["msg"] = "绑定支付宝失败";
	}
   
    out["result"] = bBindOk ? 1 : 0;
    
    // php里面把这两个搞反了，为了兼容客户端，只能继续了。。。  ,代码好像没反了
    out["alipay"] = alipay;
    out["aliname"] = aliname;
    //out = ret.toStyledString();

    return status_ok;
}
