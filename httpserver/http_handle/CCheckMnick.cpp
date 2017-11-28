#include "CCheckMnick.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Helper.h"

int CCheckMnick::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    out["result"] = 1;
    
    std::string username = root["param"]["mnick"].asString();
	if (!isNickNameInRule(username))
	{
        out["result"] = 0;
    }
//	else
//	{
		//ToLower(username);
		//username = Helper::filterInput(username);
		//Loader_Tcp::callServer2CheckName()->checkUserName(username);//调C++词库看用户名是否合法
//	}
    //out = write.write(ret);
    return status_ok;
}























