#ifndef _CRegBindingAliPay_h_
#define _CRegBindingAliPay_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CRegBindingAliPay : public CHttpRequestHandler
{
public:
    CRegBindingAliPay(){}
    ~CRegBindingAliPay(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
