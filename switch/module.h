
#ifndef module_h
#define module_h
#include <sys/types.h>
#include <assert.h>
#include <cstddef>
#include <iostream>


#define SOCKET_TCP_BUFFER  1024 * 256
#define SOCKET_ERROR		-1
#define SOCKET_CLOSE		0

namespace KernelEngine {
    
    /**
     * 网络引擎 接口定义
     */
    class ITCPNetworkEngine
    {
    public:
        virtual int getCurrentPort() = 0;
        
    public:
        // 设置参数
		virtual bool setServiceParameter(int nServicePort, size_t nMaxConnect, const std::string &serviceip, const std::string &forwardIp, int nForwardPort) = 0;
        
    public:
        // 发送数据
        virtual bool sendData(int nSocketID, const void *pData, size_t nDataSize) = 0;
        
        // 开始服务
        virtual bool startServices() = 0;
        
        // 结束服务
        virtual bool closeServices() = 0;
        
    public:
        // 关闭套接字
        virtual bool CloseSocket(int nSocketID) = 0;
    };
    
    
        

};
#endif /* module_h */
