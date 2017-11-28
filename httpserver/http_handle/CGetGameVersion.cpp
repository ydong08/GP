#include "CGetGameVersion.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "RedisLogic.h"

int CGetGameVersion::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	Json::FastWriter jWriter;
	//Json::Value ret;
	out["result"] = 0;
    int productid = Json::SafeConverToInt32(root["productid"]);
	int nType = Json::SafeConverToInt32(root["ctype"]); //客户端类型
	if (root["productid"].isNull() || productid < 0)
	{
		out["msg"] = "参数不正确";
		//out = jWriter.write(ret);
		return status_param_error;
	}

    RedisAccess* comm = RedisLogic::GetRdCommon("slave");
	if (NULL == comm)
	{
		out["msg"] = "连接redis出错";
		//out = jWriter.write(ret);
		return status_user_error;
	}
    string ver;
    bool retcode = comm->GET(StrFormatA("productVersion|%d", productid).c_str(), ver);
    if (!retcode)
	{
		out["result"] = 1;
        out["status"] = 1;
        out["version"] = 0;
        out["url"] = "";
        out["update"] = 0;
    	
    	const std::string strTb_Product_version = "zq_admin.product_version";
        string sql = StrFormatA("SELECT * FROM %s WHERE productid='%d' AND type =3 ORDER BY `version` DESC LIMIT 1", 
        	strTb_Product_version.c_str(), productid);
        MySqlDBAccess* mydb = ThreadResource::getInstance().getDBConnMgr()->DBSlave();
        MySqlResultSet* version = mydb->Query(sql.c_str());
        if (version && version->hasNext()) 
		{
            out["version"] = out["versions"] = version->getIntValue("version");
            out["url"] = version->getCharValue("url");
            out["update"] = version->getCharValue("update");
        }
        delete version;
		if (nType == 2) 
		{
			out["version"] = 0;
		}

        RedisLogic::GetRdCommon()->SET(StrFormatA("productVersion|%d", productid).c_str(), jWriter.write(out).c_str(), 24*3600);
    } 
	else 
	{
		Json::Reader jReader;
		jReader.parse(ver.c_str(), out);

		if (out["versions"].isNull()) //兼容之前少写了versions功能
		{
			out["versions"] = out["version"];
			// 以前写入的，out["versions"]没写入，先删除了，下次读数据库就正常了
			RedisLogic::GetRdCommon()->DEL(StrFormatA("productVersion|%d", productid).c_str());
		}
    }

    return status_ok;
}
