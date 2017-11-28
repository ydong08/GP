#ifndef _CGuestBinding_h_
#define _CGuestBinding_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGuestBinding : public CHttpRequestHandler
{
public:
    CGuestBinding(){}
    ~CGuestBinding(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
