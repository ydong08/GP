#ifndef _CBankResetPasswd_h_
#define _CBankResetPasswd_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CBankResetPasswd : public CHttpRequestHandler
{
public:
    CBankResetPasswd(){}
    ~CBankResetPasswd(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
