#ifndef _CRegGetPassword_h_
#define _CRegGetPassword_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CRegGetPassword : public CHttpRequestHandler
{
public:
    CRegGetPassword(){}
    ~CRegGetPassword(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
