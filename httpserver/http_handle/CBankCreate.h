#ifndef _CBankCreate_h_
#define _CBankCreate_h_
#pragma once

#include "CHttpRequestHandler.h"
#include <string>


class CBankCreate : public CHttpRequestHandler
{
public:
    CBankCreate(){}
    ~CBankCreate(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
