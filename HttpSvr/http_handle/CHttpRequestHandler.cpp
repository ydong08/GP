#include "CHttpRequestHandler.h"
#include "Logger.h"

using namespace std;


//************************************
// Method:    getDeviceNo	获取驱动序列号 ？
// FullName:  CHttpRequestHandler::getDeviceNo
// Access:    public 
// Returns:   std::string
// Qualifier:
// Parameter: Json::Value & param	客户端提交过来的json
// Parameter: std::string & strClientIp	客户端ip地址
//************************************
std::string CHttpRequestHandler::getDeviceNo(const Json::Value& param,std::string& strClientIp)
{
	std::string device_no = "";
	if (param["ctype"].isNull() || !param["ctype"].isInt()) return device_no;

	int nType = Json::SafeConverToInt32(param["ctype"]);
	switch (nType) 
	{
	case 1:
		if (!param["param"]["device_no"].isNull())
		{
			std::string strDeviceNoTmp = param["param"]["device_no"].asString();
			if (strDeviceNoTmp != "111111111111111" && strDeviceNoTmp != "Unknown")
			{
				device_no = strDeviceNoTmp;
			}
		}

		if (device_no.empty())
		{
			device_no = !param["param"]["uuid"].asString().empty() ? param["param"]["uuid"].asString() : strClientIp;
		}
		break;
	case 2:
	case 3:
		if(!param["param"]["advertising"].isNull()) device_no = param["param"]["advertising"].asString();
		if (device_no == "00000000-0000-0000-0000-000000000000") 
		{
			if(!param["param"]["openudid"].isNull()) device_no = param["param"]["openudid"].asString();
		}
		//device_no = std::str_replace(":","",device_no);
		StrReplaceAll(device_no,":","");
		if (device_no.empty()) device_no = strClientIp;
		break;
	case 4:
		device_no = strClientIp;
		break;
	}
	return device_no;
}

//************************************
// Method:    isMobile	检测参数的值是否为正确的中国手机号码格式 
// FullName:  CHttpRequestHandler::isMobile
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: std::string strPhone	手机号码
// Parameter: std::string strtype	号码类型
//************************************
bool CHttpRequestHandler::isMobile(std::string &strPhone,std::string strtype)
{   
	strPhone = TrimStr(strPhone);
	if(strPhone.empty()) return false;
	//if ("CHN" == strtype) return regex_match (strPhone, "^1([0-9]{9})");
	if ("CHN" == strtype) return regex_match (strPhone, "^((1[3,5,8][0-9])|(14[5,7])|(17[0,1,6,7,8]))[0-9]{8}$");	//这个判断更准确
	
	if ("INT" == strtype) return regex_match (strPhone, "^((\\([0-9]{3}\\))|([0-9]{3}\\-))?[0-9]{6,20}$");
	return false;
}

bool CHttpRequestHandler::isEmail(std::string &strEmail)
{
    strEmail = TrimStr(strEmail);
    if (strEmail.empty()) return false;
    return regex_match(strEmail, "[a-zA-Z0-9_.-]+@[a-zA-Z0-9_.-]+\\.[a-zA-Z0-9_.-]+");
}

bool CHttpRequestHandler::isUserName(std::string &strUserName) 
{
    strUserName = TrimStr(strUserName);
	if(strUserName.empty()) return false;
    return regex_match(strUserName, "^[a-zA-Z0-9_]{3,30}$");
}


bool CHttpRequestHandler::isPasswordInRule( std::string &strPassword )
{
	strPassword = TrimStr(strPassword);
	if(strPassword.empty()) return false;
	return regex_match(strPassword, "^[a-zA-Z0-9]{6,}$");
}

bool CHttpRequestHandler::isNickNameInRule( std::string &strNickName )
{
	int nLen = strNickName.length();	//utf8 7个汉字 = 21个字符
	if (nLen > 21 || nLen < 3) return false;
	if ((strNickName.find("*") != std::string::npos) || (strNickName.find("金") != std::string::npos) 
		|| (strNickName.find("币") != std::string::npos) || (strNickName.find("售") != std::string::npos) 
		|| (strNickName.find("卖") != std::string::npos) || (strNickName.find("Q") != std::string::npos) 
		|| (strNickName.find("q") != std::string::npos)) 
	{
		return false;
	}
	return true;
}

void CHttpRequestHandler::makeErrorResp(int errno, std::string &resp) 
{
	Json::Value 	 value;
	Json::FastWriter writer;
	value["time"] = (int64_t)time(NULL);
	value["flag"] = errno;
	resp = writer.write(value);
}

void CHttpRequestHandler::makeRet(int flag, HttpResult &out)
{
    Json::Value info;
    info["data"] = out;
    info["time"] = (int64_t)time(NULL);
    info["flag"] = flag;
    out = info;
}
