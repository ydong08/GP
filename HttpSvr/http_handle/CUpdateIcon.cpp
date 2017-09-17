#include "CUpdateIcon.h"
#include "json/json.h"
#include "Logger.h"

int CUpdateIcon::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
	out["status"] = 1;
	out["url"] = "http://139.129.201.216:8011/api.php";	//返回实际接口
	out["msg"] = "暂时不提供该接口";

    return status_ok;
}
