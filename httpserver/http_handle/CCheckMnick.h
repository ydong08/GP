#ifndef _CCHECK_MNICK_h_
#define _CCHECK_MNICK_h_

#include <string>
#include "CHttpRequestHandler.h"

class CCheckMnick : public CHttpRequestHandler
{
public:
    CCheckMnick(){}
    ~CCheckMnick(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);
};

#endif
