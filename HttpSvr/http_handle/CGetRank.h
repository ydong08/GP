#ifndef _CGetRank_h_
#define _CGetRank_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetRank : public CHttpRequestHandler
{
public:
    CGetRank(){}
    ~CGetRank(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
