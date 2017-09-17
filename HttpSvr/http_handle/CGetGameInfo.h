#ifndef _CGetGameInfo_h_
#define _CGetGameInfo_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetGameInfo : public CHttpRequestHandler
{
public:
    CGetGameInfo(){}
    ~CGetGameInfo(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
