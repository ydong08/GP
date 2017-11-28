#ifndef _CSetDefaultIcon_h_
#define _CSetDefaultIcon_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CSetDefaultIcon : public CHttpRequestHandler
{
public:
    CSetDefaultIcon(){}
    ~CSetDefaultIcon(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
