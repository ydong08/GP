#include "TCPNetworkEngine.hpp"
#include <signal.h>
#include <iostream>

using KernelEngine::TCPNetworkEngine;

TCPNetworkEngine networkengine;

void signalFunc(int _signal)
{
	if (_signal == SIGINT)
	{
		std::cout << "sigint event" << std::endl;
		networkengine.closeServices();
        exit(0);
	}
}

int main(int argc, char *argv[])
{

    if (argc < 5) {
		std::cout << "参数不正确,如(forward 127.3.2.1 7895 127.0.0.1 2000)(forward 监听IP 监听端口 被代理的服务器IP 被代理的端口)" << std::endl;
        return -1;
    }
    
   // signal(SIGINT, signalFunc);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    int forwardPort = atoi(argv[4]);
    
	int port = atoi(argv[2]);
    
    std::cout << argv[4] << std::endl << forwardPort << std::endl;
	
	networkengine.readPortConfig("portconfig");
	networkengine.setServiceParameter(port, 1000, argv[1], argv[3], forwardPort);
	networkengine.startServices();

    return 0;
}

