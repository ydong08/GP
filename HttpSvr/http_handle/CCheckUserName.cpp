#include "CCheckUserName.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Helper.h"

int CCheckUserName::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value 	 ret;
    
    std::string username = Helper::filterInput(root["param"]["username"].asString());

    if (username.empty() || isMobile(username) || !isUserName(username)) 
	{
        out["result"] = -1;
        out["msg"]    = "用户名格式错误";
        //out = write.write(ret);
        return status_ok;
    }
    
    if (hasUsername(username)) 
	{
        out["result"] = -2;
        out["msg"]    = "用户名已存在";
        //out = write.write(ret);
        return status_ok;
    }
    out["result"] = 1;
    out["msg"]    = "用户名正确";
    //out = write.write(ret);
    return status_ok;
}

bool CCheckUserName::hasUsername(std::string &username) 
{
    MySqlDBAccess* pDBUcenter = ThreadResource::getInstance().getDBConnMgr()->DBSlave();
	if (NULL == pDBUcenter) return false;
    
	std::string strSql = StrFormatA("SELECT count(username) FROM ucenter.uc_register_username WHERE username='%s' LIMIT 1",username.c_str());
    
    MySqlResultSet *pResult = pDBUcenter->Query( strSql.c_str() );
    if ( pResult == NULL ) return false;
    
    int count = 0;
    if (pResult->hasNext()) count = pResult->getIntValue(0);
    
	delete pResult;
    return count > 0 ? true : false;
}



















