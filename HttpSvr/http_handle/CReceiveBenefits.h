//
//  CReceiveBenefits.hpp
//  HttpSvr
//
//  Created by yuzhan on 2016/11/14.
//
//

#ifndef CReceiveBenefits_hpp
#define CReceiveBenefits_hpp

#include <string>
#include "CHttpRequestHandler.h"

class CReceiveBenefits : public CHttpRequestHandler
{
public:
    CReceiveBenefits(){}
    ~CReceiveBenefits(){}
    
    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif /* CReceiveBenefits_hpp */
