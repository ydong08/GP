#ifndef _GUEST_LOGIN_H_
#define _GUEST_LOGIN_H_
#pragma once
#include "CHttpRequestHandler.h"
#include <string>

class CAccountLogin : public CHttpRequestHandler
{
public:
    CAccountLogin(){}
    ~CAccountLogin(){}

	bool IsForbidLogin(std::string& ip, std::string& device_no,std::string& openudid);
	bool IsForbidLoginByMid(int32_t Mid);
    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
