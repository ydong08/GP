#ifndef _CUPDATE_USER_H_
#define _CUPDATE_USER_H_

#include <string>
#include "CHttpRequestHandler.h"

class CUpdateUser : public CHttpRequestHandler
{
public:
    CUpdateUser(){}
    ~CUpdateUser(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
};

#endif
