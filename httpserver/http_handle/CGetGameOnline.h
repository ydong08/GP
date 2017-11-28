#ifndef _CGetGameOnline_h_
#define _CGetGameOnline_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetGameOnline : public CHttpRequestHandler
{
public:
    CGetGameOnline(){}
    ~CGetGameOnline(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
