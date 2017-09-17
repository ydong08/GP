#ifndef _CGetGameSwitch_h_
#define _CGetGameSwitch_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetGameSwitch : public CHttpRequestHandler
{
public:
    CGetGameSwitch(){}
    ~CGetGameSwitch(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
