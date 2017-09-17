#ifndef _CUpdateIcon_h_
#define _CUpdateIcon_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CUpdateIcon : public CHttpRequestHandler
{
public:
    CUpdateIcon(){}
    ~CUpdateIcon(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
