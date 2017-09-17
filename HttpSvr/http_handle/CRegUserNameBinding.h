#ifndef _CRegUserNameBinding_h_
#define _CRegUserNameBinding_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CRegUserNameBinding : public CHttpRequestHandler
{
public:
    CRegUserNameBinding(){}
    ~CRegUserNameBinding(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
