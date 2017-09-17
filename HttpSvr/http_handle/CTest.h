//
//  CTest.hpp
//  HttpSvr
//
//  Created by yuzhan on 2016/11/15.
//
//

#ifndef CTest_hpp
#define CTest_hpp


#include <string>
#include "CHttpRequestHandler.h"

class CTest : public CHttpRequestHandler
{
public:
    CTest(){}
    ~CTest(){}
    
    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
};

#endif /* CTest_hpp */
