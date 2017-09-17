#ifndef _CGetTaskInfo_h_
#define _CGetTaskInfo_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetTaskInfo : public CHttpRequestHandler
{
public:
    CGetTaskInfo(){}
    ~CGetTaskInfo(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
