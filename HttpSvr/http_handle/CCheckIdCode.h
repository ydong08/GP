//
//  CCheckIdCode.h
//  HttpSvr
//
//  Created by yuzhan on 2016/11/10.
//
//

#ifndef CCheckIdCode_H_
#define CCheckIdCode_H_

#include <string>
#include "CHttpRequestHandler.h"

class CCheckIdCode : public CHttpRequestHandler
{
public:
    CCheckIdCode(){}
    ~CCheckIdCode(){}
    
    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
