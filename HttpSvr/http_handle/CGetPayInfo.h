#ifndef _CGetPayInfo_h_
#define _CGetPayInfo_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"

class CGetPayInfo : public CHttpRequestHandler
{
public:
    CGetPayInfo(){}
    ~CGetPayInfo(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
    bool getWithdrawalsByDealNo(std::string payDealNo, Json::Value& info);
    bool getPayByDealNo(std::string payDealNo, Json::Value& info);
};

#endif
