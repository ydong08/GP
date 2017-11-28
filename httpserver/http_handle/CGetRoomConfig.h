#ifndef _CGetRoomConfig_h_
#define _CGetRoomConfig_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetRoomConfig : public CHttpRequestHandler
{
public:
    CGetRoomConfig(){}
    ~CGetRoomConfig(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
