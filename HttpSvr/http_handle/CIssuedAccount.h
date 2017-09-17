#ifndef CIssuedAccount_H_
#define CIssuedAccount_H_
#pragma once

#include "CHttpRequestHandler.h"
#include <string>


class CIssuedAccount : public CHttpRequestHandler
{
public:
    CIssuedAccount(){}
    ~CIssuedAccount(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
