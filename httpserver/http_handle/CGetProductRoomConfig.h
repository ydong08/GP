#ifndef _CGetProductRoomConfig_h_
#define _CGetProductRoomConfig_h_
#pragma once

#include <string>

#include "CHttpRequestHandler.h"


class CGetProductRoomConfig : public CHttpRequestHandler
{
public:
    CGetProductRoomConfig(){}
    ~CGetProductRoomConfig(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
