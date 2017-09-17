#include "CGetNotice.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"

int CGetNotice::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value 	 ret;
    //Json::FastWriter write;
    out["result"] = 0;
    if (root["ctype"].isNull() || root["cid"].isNull() || root["sid"].isNull() || root["productid"].isNull())
    {
		out["msg"] = "参数不正确";
		//out = write.write(ret);
		return status_argment_access;
    }
    int ctype = Json::SafeConverToInt32(root["ctype"]);
    int cid   = Json::SafeConverToInt32(root["cid"]);
    int sid   = Json::SafeConverToInt32(root["sid"]);
    int pid   = Json::SafeConverToInt32(root["productid"]);
    
    Json::Value info;
    if(!GetNotice(pid, info)) 
	{
		out["msg"] = "获取通知失败";
		//out = write.write(ret);
		return status_ok;
	}
	out["result"] = 1;
	out["msg"] = info;
	//out = write.write(ret);
	return status_ok;
}

bool CGetNotice::GetNotice(int pid, Json::Value &info) 
{
    MySqlDBAccess* pDBUcenter = ThreadResource::getInstance().getDBConnMgr()->DBSlave();
    if (NULL == pDBUcenter) return false;

    int now = time(NULL);
	std::string strSql = StrFormatA("SELECT title, content, url, ctime FROM ucenter.uc_notice WHERE %d > stime AND %d < etime AND productid=%d ORDER BY ctime DESC",
		now, now, pid);
    
    MySqlResultSet *pResult = pDBUcenter->Query( strSql.c_str() );
    if ( pResult == NULL ) return false;
    
    while(pResult->hasNext()) 
	{
        Json::Value node;
        node["title"]   = std::string(pResult->getCharValue(0));
        node["content"] = std::string(pResult->getCharValue(1));
        node["url"]     = std::string(pResult->getCharValue(2));
        node["ctime"]   = pResult->getIntValue(3);
        info.append(node);
    }
	delete pResult;
    return true;
}
























