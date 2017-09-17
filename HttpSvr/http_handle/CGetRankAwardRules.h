#ifndef _CGetRankAwardRules_h_
#define _CGetRankAwardRules_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetRankAwardRules : public CHttpRequestHandler
{
public:
    CGetRankAwardRules(){}
    ~CGetRankAwardRules(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
