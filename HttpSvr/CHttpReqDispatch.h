#ifndef HTTP_REQ_DISPATCH_H_
#define HTTP_REQ_DISPATCH_H_
#pragma once

#include <string>
#include <event.h>
#include <evhttp.h>
#include "http_handle/CHttpRequestHandler.h"

int handleReq(char* recvbuf, unsigned int rcvlen, const char* uri, char *client_ip, u_short client_port, HttpResult& out);
class  CHttpReqDispatch
{
   
public:
    CHttpReqDispatch(){}
    ~CHttpReqDispatch();
	
	static CHttpReqDispatch& getInstance()
	{
		static CHttpReqDispatch m_ins;
		return m_ins;
	}

    void register_handler();
    CHttpRequestHandler* getHttpRequestHandler(const std::string& method);
    int DispatchReq(const char* recvbuf, unsigned int rcvlen, const char* uri, const char *client_ip, std::string& strResult);

private:
    std::map<std::string, CHttpRequestHandler*> m_handlers;
};


#endif
