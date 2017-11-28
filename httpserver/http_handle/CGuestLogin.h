#ifndef _CGuestLogin_h_
#define _CGuestLogin_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGuestLogin : public CHttpRequestHandler
{
public:
    CGuestLogin(){}
    ~CGuestLogin(){}

	virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
	bool IsForbidLogin(std::string& ip, std::string& device_no,std::string& openudid);
	bool IsForbidLoginByMid(int32_t Mid);
};

#endif
