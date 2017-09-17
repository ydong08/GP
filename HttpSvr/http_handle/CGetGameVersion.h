#ifndef _CGetGameVersion_h_
#define _CGetGameVersion_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetGameVersion : public CHttpRequestHandler
{
public:
    CGetGameVersion(){}
    ~CGetGameVersion(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
