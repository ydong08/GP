#ifndef _CGetFeedBackHistory_h_
#define _CGetFeedBackHistory_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetFeedBackHistory : public CHttpRequestHandler
{
public:
    CGetFeedBackHistory(){}
    ~CGetFeedBackHistory(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
