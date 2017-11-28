#ifndef _CBankFindPasswd_h_
#define _CBankFindPasswd_h_

#include "CHttpRequestHandler.h"

class CBankFindPasswd : public CHttpRequestHandler
{
public:
    CBankFindPasswd(){}
    ~CBankFindPasswd(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
    int getCacheIdCode(std::string codeKey);
    bool setUserBackPassword(int mid);
};
 
#endif
