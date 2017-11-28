#ifndef _CGetFeedBackSet_h_
#define _CGetFeedBackSet_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetFeedBackSet : public CHttpRequestHandler
{
public:
    CGetFeedBackSet(){}
    ~CGetFeedBackSet(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
