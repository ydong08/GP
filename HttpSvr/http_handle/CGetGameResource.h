#ifndef _CGetGameResource_h_
#define _CGetGameResource_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetGameResource : public CHttpRequestHandler
{
public:
    CGetGameResource(){}
    ~CGetGameResource(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
