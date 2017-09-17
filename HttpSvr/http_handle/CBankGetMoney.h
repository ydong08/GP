#ifndef _CBankGetMoney_h_
#define _CBankGetMoney_h_
#pragma once

#include "CHttpRequestHandler.h"
#include <string>


class CBankGetMoney : public CHttpRequestHandler
{
public:
    CBankGetMoney(){}
    ~CBankGetMoney(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
