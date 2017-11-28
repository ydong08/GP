#ifndef _CRegUserName_h_
#define _CRegUserName_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CRegUserName : public CHttpRequestHandler
{
public:
    CRegUserName(){}
    ~CRegUserName(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
