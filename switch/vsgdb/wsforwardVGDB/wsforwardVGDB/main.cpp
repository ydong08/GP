#include "TCPNetworkEngine.hpp"
#include <signal.h>
#include <iostream>
#include "Logger.h"

using KernelEngine::TCPNetworkEngine;

TCPNetworkEngine networkengine;


void signalFunc(int _signal)
{
	LOGGER(E_LOG_DEBUG) << "sigint event";
	networkengine.closeServices();
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
		CLogger::InitLogger( E_LOG_INFO, "wsForward", 0);
		LOGGER(E_LOG_FATAL) << "参数不正确,如(forward 7895 127.0.0.1 2000 1)(forward 监听端口 被代理的服务器IP 被代理的端口 日志级别(0 ~4))";
        return -1;
    }
    
	signal(SIGINT, signalFunc);
	signal(SIGTERM, signalFunc);

	unsigned char nLogLevel = E_LOG_INFO;
	if (5 == argc) 
	{
		nLogLevel = atoi(argv[4]);
		if (nLogLevel > E_LOG_FATAL) nLogLevel = E_LOG_INFO;
	}
	
	CLogger::InitLogger( nLogLevel, "wsForward", 0);

    int forwardPort = atoi(argv[3]);
    
	int port = atoi(argv[1]);
    
	LOGGER(E_LOG_INFO) << "listen port: " << port << ",forward info:" << argv[2] << " : " << forwardPort;
	
	networkengine.setServiceParameter(port, argv[2], forwardPort);
	networkengine.startServices();

    return 0;
}
