#ifndef _CHttpRequestHandler_h_
#define _CHttpRequestHandler_h_
#pragma once

#include "json/json.h"
#include <string>
#include "StrFunc.h"
#include "JsonValueConvert.h"
#include "UserDoing.h"

//typedef std::string HttpResult;
typedef Json::Value HttpResult;

static const int status_ok			   = 200; //处理成功
static const int status_check_error    = 100; //权限检查错误
static const int status_param_error	   = 101; //参数错误
static const int status_argment_access = 102; //缺少参数
static const int status_method_error   = 103; //方法错误
static const int status_register_error = 104; //玩家注册失败
static const int status_user_error     = 105; //获取玩家信息失败
static const int status_faild_access   = 106; //被封号
static const int status_file_error     = 107; //上传文件格式错误
static const int status_account_error  = 108; //用户名或密码不正确
static const int status_vercode_error  = 109; //验证码出错

class CHttpRequestHandler
{
public:
    CHttpRequestHandler(){}
    virtual ~CHttpRequestHandler(){}
    
	virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out) = 0;
    virtual void makeRet(int flag, HttpResult& out);
	void makeErrorResp(int errno, std::string &resp);

	std::string getDeviceNo(const Json::Value& param, std::string& strClientIp);
	bool isMobile(std::string &strPhone,std::string strtype = "CHN");
    bool isEmail(std::string& strEmail);
	bool isUserName(std::string &strUserName);
	bool isPasswordInRule(std::string &strPassword);
	bool isNickNameInRule(std::string &strNickName);
    
private:
};
#endif 
