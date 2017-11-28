#ifndef _CCHECK_USERNAME_h_
#define _CCHECK_USERNAME_h_

#include <string>
#include "CHttpRequestHandler.h"

class CCheckUserName : public CHttpRequestHandler
{
public:
    CCheckUserName(){}
    ~CCheckUserName(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
private:
    bool hasUsername(std::string &username);
};

#endif //end for _CCHECK_USERNAME_h_
