#include "CRegGetLdCode.h"
#include "json/json.h"
#include "Logger.h"
#include "Helper.h"
#include "threadres.h"
#include "HttpLib.h"
#include "RedisLogic.h"
#include "SqlLogic.h"

int getOneByPhone(string phone, int productid) 
{
    string query = StrFormatA("SELECT sitemid FROM ucenter.uc_account_binding WHERE phoneno = '%s' and productid='%d'",
                            phone.c_str(), productid);
    MySqlResultSet * res = ThreadResource::getInstance().getDBConnMgr()->DBMaster()->Query(query.c_str());
    int nrecord = 0;
    if (res)
	{
		if (res->hasNext())
		{
			nrecord = res->getRowNum();
		}
		delete res;
	}
    return nrecord;
}


int CRegGetLdCode::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
    Json::FastWriter jWriter;
    out["result"] = 0;
    string phoneno, sitemid;
    int type, productid;
    try {
        phoneno       = root["param"].isMember("phoneno") ? Helper::filterInput(root["param"]["phoneno"].asString()) : "";
        sitemid       = root["param"]["sitemid"].asString();
        type          = root["param"]["type"].asInt();
        productid = root["productid"].asInt();
	} catch (...) {
		out["result"] = -1;
        out["msg"] = "缺少参数";
		//out = jWriter.write(ret);
		return status_ok;
    }

	if(phoneno.empty()){
		out["result"] = -1;
        out["msg"] = "手机号为空";
        //out = jWriter.write(ret);
        return status_ok;
    }

    if(!isMobile(phoneno)){
        out["result"] = -1;
        out["msg"] = "手机号格式错误";
        //out = jWriter.write(ret);
        return status_ok;
    }

    int count = 0;
    RedisAccess *common = ThreadResource::getInstance().getRedisConnMgr()->common();
    string value;
    bool retcode = common->GET(("ID"+phoneno).c_str(), value);
    if (retcode)
        count = atoi(value.c_str());

    if(count == 1){//限定60S内不能重发
        out["result"] =  -1;
        out["msg"] =  "发送次数过于频繁";
        //out = jWriter.write(ret);
        return status_ok;
    }

    if(type == 2){//手机号码绑定
        if(sitemid.empty()){
            //out = jWriter.write(ret);
            return status_ok;
        }

        bool status = SqlLogic::userNameBinding(atoi(sitemid.c_str()), phoneno,productid);
        if(!status){//已经绑定或已经被注册
            out["result"] =  -3;
            //out = jWriter.write(ret);
            return status_ok;
        }

        if(RedisLogic::limitCount(sitemid, StrFormatA("%d",100), 1,true,Helper::time2morning()) > 5){
            out["result"] =  -2;
            //out = jWriter.write(ret);
            return status_ok;
        }
    }

    if(type == 1){//获取验证码
        if(RedisLogic::limitCount(phoneno, StrFormatA("%d",102), 1,true,Helper::time2morning()) > 5){
            out["result"] =  -2;
            //out = jWriter.write(ret);
            return status_ok;
        }
    }

    int idcode = 100000 + rand()%899999;
    bool flag   = RedisLogic::setCodeCheck(phoneno, idcode);

    // 找回密码获取验证码
    if (flag && type == 3) {
        int rec = getOneByPhone(phoneno, productid);
        if (!rec) {
            out["result"] =  -3;
            out["msg"] =  "该手机未绑定账号";
            //out = jWriter.write(ret);
            return status_ok;
        }
        common->SET(("password"+phoneno).c_str(), 1,10*60);
    }

    if(flag){
        bool result = HttpLib::sendMessage(Json::Value(StrFormatA("%d",idcode).c_str()), phoneno, 1);
        out["result"] = 1;
        //out["code"] = "******";
        common->SET(("ID"+phoneno).c_str(), 1,60);
    }

    out["type"] = type;
    //out = jWriter.write(ret);
    return status_ok;
}
