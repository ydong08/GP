#ifndef _CGetDrawGold_h_
#define _CGetDrawGold_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

//抽金币
class CGetDrawGold : public CHttpRequestHandler
{
public:
    CGetDrawGold(){}
    ~CGetDrawGold(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
