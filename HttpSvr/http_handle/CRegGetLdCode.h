#ifndef _CRegGetLdCode_h_
#define _CRegGetLdCode_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CRegGetLdCode : public CHttpRequestHandler
{
public:
    CRegGetLdCode(){}
    ~CRegGetLdCode(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
