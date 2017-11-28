#ifndef _CGetTaskAward_h_
#define _CGetTaskAward_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetTaskAward : public CHttpRequestHandler
{
public:
    CGetTaskAward(){}
    ~CGetTaskAward(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
