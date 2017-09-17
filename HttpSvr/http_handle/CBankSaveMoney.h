#ifndef _CBankSaveMoney_h_
#define _CBankSaveMoney_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CBankSaveMoney : public CHttpRequestHandler
{
public:
    CBankSaveMoney(){}
    ~CBankSaveMoney(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
