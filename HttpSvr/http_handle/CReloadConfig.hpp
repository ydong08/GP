//
//  CReloadConfig.hpp
//  HttpSvr
//
//  Created by yuzhan on 2016/11/19.
//
//

#ifndef CReloadConfig_hpp
#define CReloadConfig_hpp


#include <string>
#include "CHttpRequestHandler.h"

class CReloadConfig : public CHttpRequestHandler
{
public:
    CReloadConfig(){}
    ~CReloadConfig(){}
    
    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
};

#endif /* CReloadConfig_hpp */
