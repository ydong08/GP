#ifndef _CGetWinGoldRank_h_
#define _CGetWinGoldRank_h_
#pragma once

#include <string>
#include "CHttpRequestHandler.h"


//活动金币排行

class CGetWinGoldRank : public CHttpRequestHandler
{
public:
    CGetWinGoldRank(){}
    ~CGetWinGoldRank(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
