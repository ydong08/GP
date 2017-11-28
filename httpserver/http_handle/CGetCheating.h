#ifndef _CGetCheating_h_
#define _CGetCheating_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetCheating : public CHttpRequestHandler
{
public:
    CGetCheating(){}
    ~CGetCheating(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
