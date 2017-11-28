#ifndef __CGET_NOTICE_H_
#define __CGET_NOTICE_H_

#include <string>
#include "CHttpRequestHandler.h"

class CGetNotice : public CHttpRequestHandler
{
public:
    CGetNotice(){}
    ~CGetNotice(){}

    virtual int do_request(const Json::Value& root, char *client_ip, HttpResult& out);

private:
    bool GetNotice(int pid, Json::Value &info); 
};

#endif //end for __CGET_NOTICE_H_
