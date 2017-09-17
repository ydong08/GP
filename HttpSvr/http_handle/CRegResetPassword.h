#ifndef _CRegResetPassword_h_
#define _CRegResetPassword_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CRegResetPassword : public CHttpRequestHandler
{
public:
    CRegResetPassword(){}
    ~CRegResetPassword(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
