/**
*
* ━━━━━━神兽出没━━━━━━
* 　　　┏┓　　　┏┓
* 　　┏┛┻━━━┛┻┓
* 　　┃　　　　　　　┃
* 　　┃　　　━　　　┃
* 　　┃　┳┛　┗┳　┃
* 　　┃　　　　　　　┃
* 　　┃　　　┻　　　┃
* 　　┃　　　　　　　┃
* 　　┗━┓　　　┏━┛Code is far away from bug with the animal protecting
* 　　　　┃　　　┃    神兽保佑,代码无bug
* 　　　　┃　　　┃
* 　　　　┃　　　┗━━━┓
* 　　　　┃　　　　　　　┣┓
* 　　　　┃　　　　　　　┏┛
* 　　　　┗┓┓┏━┳┓┏┛
* 　　　　　┃┫┫　┃┫┫
* 　　　　　┗┻┛　┗┻┛
*
* ━━━━━━感觉萌萌哒━━━━━━
*/
//
//  TCPNetworkEngine.hpp
//  websocket_forward_prj
//
//  Created by 水深 on 16/11/15.
//  Copyright © 2016年 水深. All rights reserved.
// 游戏客户端 --- 转发服务器 ---游戏服务器

#ifndef TCPNetworkEngine_hpp
#define TCPNetworkEngine_hpp
#include "module.h"
#include <vector>
#include "SThread.hpp"
#include <mutex>
#include <time.h>
#include <sys/epoll.h>
#include <string>
#include <map>
#include "Base64.h"
#include "sha1.h"
#include "MemoryPool.h"

//索引辅助
#define SOCKET_INDEX(dwSocketID)  (dwSocketID >> 16)         //位置索引
#define SOCKET_ROUNTID(dwSocketID)  (dwSocketID & 0xFFFF)         //循环索引

#define NTOHLL(UL) ntohl((UL)& 0xFFFFFFFF) << 16 + ntohl((UL) >> 16) 
#define HTONLL(UL) htonl((UL)& 0xFFFFFFFF) << 16 + htonl((UL) >> 16)

#define NETWORKIEM_MEMORY_SIZE 1024 * 1024 * 512

#define MAN_EPOLL_EVENT 3000

#define MAX_LISTEN_PORT 20

namespace KernelEngine {
    
    class TCPNetworkItem;
    class TCPNetworkThreadAccept;
	class TCPNetworkThreadClientRW;
	class TCPNetworkItemClient;
	class TCPNetworkItemForward;
	class TCPNetworkEngine;
	class TCPNetworkThreadForwardRW;
    class TCPNetworkDataCache;
    
    
    enum SendResult {
        SEND_SUCCESS,
        SEND_FULL,
        SEND_ERROR
    };

	class ITCPNetworkItemClient
	{
	public:
		virtual bool onEventForwardBindSuccess() = 0;
		virtual bool onEventForwardBindFailure() = 0;
	};
    
    // 网络事件回调接口
    class ITCPNetworkItemSink
    {
    public:
        // 绑定套接字
        virtual bool onEventSocketBind(TCPNetworkItemClient *pTCPNetworkItem) = 0;
        
        // 读取数据
		virtual bool onEventSocketRead(TCPNetworkItemClient *pTCPNetworkItem, void *pData, ushort usDataSize) = 0;

		// 转发端读取数据
		virtual bool onEventForwardSocketRead(TCPNetworkItemForward *pTCPNetworkItem, void *pData, ushort usDataSize) = 0;

		// 客户端关闭套接字
		virtual bool onEventClientShutdown(TCPNetworkItemClient *pTCPNetworkItem) = 0;

		// 转发端口关闭套接字
		virtual bool onEventForwardShutdown(TCPNetworkItemForward *pTCPNetworkItem) = 0;
        
        virtual bool modofiClientEvent(TCPNetworkItem *clientItem, int events) = 0;
        virtual bool modifiForwardEvent(TCPNetworkItem *clientItem, int events) = 0;
    };
    
    // 加密解密网络协议数据
    class IEncode
    {
	public:
		// 加密数据
		virtual bool encodeData(const void *pData, int wDataSize, void *pNewData, int *pNewDataSize) = 0;
		virtual bool decodeData(const void *pData, int size, void *pNewData, int *pNewDataSize) = 0;
    };
    
	struct ForwardStruct
	{
		TCPNetworkItemClient *pTCPNetworkItemClient;
		TCPNetworkItemForward *pTCPNetworkItemForward;
	};


	//  WebSocket协议
	#define  WebsocketHeadSize 2

	struct WebsocketHead
	{
		char fin;
		char mask;
	};

	struct WebsocketExt
	{
		short len;
		//int32_t mask;
	};

	struct WebsocketExt2
	{
		unsigned long long len;
		//int32_t mask;
	};


	class WebSocketProtocol : public IEncode
	{
	public:
		bool parseWebsocketHead(void *pData, ushort wDataSize);
		bool checkWebsocketRecvFinish(void *pData, ushort wDataSize, ushort *endSize);
		const char *getSendBrowseKeyData(int *size);


		// 加密数据
		virtual bool encodeData(const void *pData, int wDataSize, void *pNewData, int *pNewDataSize);

		// 解密数据
		virtual bool decodeData(const void *pData, int size, void *pNewData, int *pNewDataSize);

		// 检测包是否接受完成
		virtual bool checkPacketFinish(void *pData, ushort wDataSize);

		// 获取数据包大小
		virtual int getPacketSize(void *pData, ushort wDataSize);

		// 获取数据包头大小
		virtual int getPacketHeadSize();

		virtual int getPacketTrueHeadSize(char fmask);
	private:
		// 信息头
		std::map<std::string, std::string> _headInfoMap;
		std::string sendHead;
	};
    

	/**
	* 连接线程
	*/
	class TCPNetworkThreadAccept : public SThread
	{
	public:
		static const size_t kMaxEvents = MAN_EPOLL_EVENT;
	protected:
		
		int _epoll_fd = -1;          // epoll描述符 用于客户端连接
		int _socket_fd[MAX_LISTEN_PORT] = { -1 };          // 服务器套接字描述符号
		TCPNetworkEngine *_pNetworkEngine = nullptr;

	public:
		bool InitThread(int socket_fd[], int epoll_fd, TCPNetworkEngine *pNetworkEngine);

	private:
		virtual bool onEventThreadRunOne(int sockfd);//单个socket监听线程
		virtual bool onEventThreadRun();        // 线程正在运行
		virtual void onEventThreadStart();      // 线程开始运行

	private:
		epoll_event events[TCPNetworkThreadAccept::kMaxEvents];
	};

	/**
	* 客户端读写线程
	*/
	class TCPNetworkThreadClientRW : public SThread
	{
	protected:
		static const size_t kMaxEvents = MAN_EPOLL_EVENT;
		int _epoll_fd = -1;          // epoll描述符 用于客户端读写

	public:
		bool InitThread(int epoll_fd);

	public:
		virtual bool onEventThreadRun();        // 线程正在运行
		virtual void onEventThreadStart();      // 线程开始运行

	private:
		epoll_event events[TCPNetworkThreadAccept::kMaxEvents];
	};


    
    /**
     * 网络引擎
     */
    class TCPNetworkEngine : public ITCPNetworkEngine, public ITCPNetworkItemSink
    {
		friend class TCPNetworkThreadAccept;
		friend class TCPNetworkThreadClientRW;

		typedef std::vector<ForwardStruct *> TCPNetworkItemVector;
		std::map<int, int> sockport;
    private:
		const int nEpollCreateMax = MAN_EPOLL_EVENT;
        const int nMaxConnect = MAN_EPOLL_EVENT;
        const size_t _nMemoryBufferSize = 1024 * 1024; // 缓存池的大小
        char *_memorybuffer = nullptr;
        
    public:
		std::recursive_mutex  _mutex;           // 条目互斥
        int getCurrentPort();
        
        TCPNetworkEngine();
        ~TCPNetworkEngine();
        
    public:
        // 设置参数
        bool setServiceParameter(int nServicePort, size_t nMaxConnect, const std::string &serviceip, const std::string &forwardIp, int nForwardPort);
		//读取配置的服务器端口
		bool readPortConfig(std::string confile);
        
    public:
        // 发送数据
		bool sendData(int32_t nSocketID, const void *pData, size_t nDataSize);
        
        // 开始服务
        bool startServices();
        
        // 结束服务
        bool closeServices();
        
    public:
        // 关闭套接字
        virtual bool CloseSocket(int nSocketID);

		// 获取客户端条目
		TCPNetworkItemClient *getNetworkItem(int index);

		// 移除客户端和转发端在epoll中的监听
		virtual bool removeItemEvent(TCPNetworkItemClient *, TCPNetworkItemForward *);
        
    private:
        int createServerSock(int port);

		bool createEpoll(); // 创建epoll
		bool registerEpoll(int epoll_fd, int sockfd); // 注册epoll
		bool registerEpollClient(int epoll_fd, TCPNetworkItem *pNetworkItem); // 注册客户端epoll
		bool removePollEvent(int epoll_fd, int sockfd); // 删除epoll里面注册的描述符
		bool removeClientEpollEvent(int sockfd); // 删除客户端epoll 监听
		bool removeForwardEpollEvent(int sockfd); // 删除转发端epoll 监听
        
    public:
        bool modofiClientEvent(TCPNetworkItem *clientItem, int events);
        bool modifiForwardEvent(TCPNetworkItem *clientItem, int events);
		
        
    private:
        int _nPort; // 端口号
		int _nForwardPort; // 转发端口号
		int _nPortArray[MAX_LISTEN_PORT];//转发服务器监听端口
		int _nSockArray[MAX_LISTEN_PORT];//转发服务器监听端口对应的socket
		std::string _serviceip;//本机IP用于替换C++服务器中的IP
		std::string _forwardIp;
        size_t _nMaxConnect;
        
        TCPNetworkItemVector _vecNetworkItemStorage; // 存储连接
        TCPNetworkItemVector _vecNetworkItemActive;  // 活动连接
        TCPNetworkItemVector _vecNetworkItemBuffer;  // 空闲连接


	private:
		TCPNetworkThreadAccept _TCPNeworkThreadAccept;
		TCPNetworkThreadClientRW _TCPNetworkThreadClientRW;
		TCPNetworkThreadClientRW _TCPNetworkThreadForwardRW;
        
        
    private:
        int _serverSocket = -1; // 服务端套接字
        int _epoll_fd = -1;     // epoll描述符 用于客户端连接
        int _client_epoll_fd = -1; // epoll描述符 用于客户端读取／写入
		int _forward_epoll_fd = -1; // epoll描述符 用于客户端读取／写入


	private:
		bool _bService = false;
        
    public:
        // 激活一个网络条目
		TCPNetworkItem *activeNetworkItem();
        
        // 绑定套接字
        bool onEventSocketBind(TCPNetworkItemClient *pTCPNetworkItem);
        
        // 读取数据
		bool onEventSocketRead(TCPNetworkItemClient *pTCPNetworkItem, void *pData, ushort usDataSize);
		bool onEventForwardSocketRead(TCPNetworkItemForward *pTCPNetworkItem, void *pData, ushort usDataSize);

		// 套接字关闭
		bool onEventClientShutdown(TCPNetworkItemClient *pTCPNetworkItem);
		bool onEventForwardShutdown(TCPNetworkItemForward *pTCPNetworkItem);

		// 获取对应的转发条目
		TCPNetworkItemForward *findForwardItem(TCPNetworkItemClient *pClientItem);
		TCPNetworkItemClient *findClientItem(TCPNetworkItemForward *pClientItem);
        
    };
    
    // 数据缓冲类
    class TCPNetworkDataCache
    {
        friend class TCPNetworkItem;
    private:
        char *_dataBuffer = nullptr; // 数据缓冲
        size_t _dataBufferSize = 0; // 数据缓冲大小
        size_t _dataBufferUseSize = 0; // 数据缓冲大小
        size_t _dataBufferIndex = 0; // 数据缓冲索引
        
        const size_t kChunkSize = 10240; // 一个内存块
        
    public:
        void resetData();
        
    public:
        bool addDataBuffer(const void *pdata, size_t size); // 添加数据缓冲
        bool skipDataBuffer(size_t size) 
		{
			if (size > _dataBufferUseSize)
			{
				std::cout << "skipDataBuffer size > _dataBufferUseSize" << std::endl;
				size = _dataBufferUseSize;
			}


            _dataBufferIndex += size; // 索引后移
            _dataBufferUseSize -= size; // 已使用的大小 变少
            return true;
        }; // 跳过数据
        bool freeDataBuffer(); // 释放内存
        char *getDataBuffer() { return _dataBuffer + _dataBufferIndex; };
        size_t getBufferLen() { return _dataBufferUseSize; };
    };
    
    /**
     * 客户端连接条目
     */
    class TCPNetworkItem
    {
        friend class TCPNetworkEngine;
    public:
        TCPNetworkItem(ushort index, ITCPNetworkItemSink *pITCPNetworkItemSink);
		virtual ~TCPNetworkItem();
		ushort  _usRountID = 0;          // 循环索引
        
        TCPNetworkDataCache _dataCache; // 数据缓冲
    protected:
        unsigned _uClientIP;        // 连接地址
        time_t _activeTime;         // 激活事件
        
    protected:
        ushort  _usIndex = 0;            // 连接索引
		bool	_bClose = true;
        ushort  _usSurvivalTime = 0;    // 生存事件
        int     _socketClient = -1;       // 客户端连接套接字
        int     _socketForward = -1;      // 转发服务器套接字
        std::shared_ptr<std::mutex>  _mutex;           // 条目互斥
        ITCPNetworkItemSink *_pITCPNetworkItemSink = nullptr; // 回调接口
        
        // 状态变量
    protected:
        bool    _bSendIng = false;           // 发送标志
        bool    _bRecvIng = false;           // 接受标志
        bool    _bShutDown = false;          // 关闭标志
        
        // 接受变量
    protected:
        ushort  _usRecvSize = 0;         // 接受数据长度
        u_char  _cbRecvBuf[SOCKET_TCP_BUFFER];        //接受缓冲
        
    protected:
        time_t  _sendTickCount = 0;      // 发送时间
        time_t  _recvTickCount = 0;      // 接受时间
        unsigned  _sendPacketCount = 0;  // 发送计数
        unsigned  _recvPacketCount = 0;  // 接受计数
        
        // 获取条目属性
    public:
        ushort getIndex() { return _usIndex; }; // 获取索引
        ushort getRountID() { return _usRountID; }; // 获取循环计数
        
        // 获取标识
        int32_t getIdentifierID()
        {
            return (int32_t)(_usIndex << 16) + _usRountID;
        };
        
        unsigned getClientIP() { return _uClientIP; }; // 获取客户端ip
        time_t getActiveTime() { return _activeTime; }; // 激活时间
        time_t getSendTickCount() { return _sendTickCount; }; // 发送时间
        time_t getRecvTickCount() { return _recvTickCount; }; // 接受时间
        
        unsigned getSendPacketCount() { return _sendPacketCount; }; // 发送包数量
        unsigned getRecvPacketCount() { return _recvPacketCount; }; // 发送包数量
        
        void forward_lock() { _mutex->lock(); }; // 加锁
        void forward_unlock() { _mutex->unlock(); }; // 解锁
        
        // 发送数据包
    public:

		virtual bool SendData(const void *pData, ushort wDataSize, ushort usRountID) { return true; };
        
        // 发送数据
        virtual SendResult _sendData(const void *pData, ushort wDataSize);
        
        // 接受数据
		virtual bool onEventEpollIn() = 0;

		// epollout
        virtual bool onEventEpollOut();
        
        // 关闭套接字
        virtual bool CloseSocket(ushort usRountID);
        
    public:
        // 重置数据
        unsigned resumeData();
        
        // 绑定客户端对象
        virtual unsigned attach(int sockfd, unsigned uClientIP);
        
    private:
        TCPNetworkItem *pForwardNetworkItem = nullptr; // 对应的转发服务器
        
    };
    
	// 客户端条目
	class TCPNetworkItemClient : public TCPNetworkItem, public ITCPNetworkItemClient
	{
		friend class TCPNetworkEngine;

	public:
		TCPNetworkItemClient(ushort index, ITCPNetworkItemSink *pITCPNetworkItemSink) 
			:TCPNetworkItem(index, pITCPNetworkItemSink) {};

	public:
		bool onEventForwardBindSuccess();
		bool onEventForwardBindFailure();
		virtual unsigned attach(int sockfd, unsigned uClientIP) override;


	public:

		virtual bool SendData(const void *pData, ushort wDataSize, ushort usRountID) override;

		// 接受数据
		virtual bool onEventEpollIn() override;

		// epollout
		virtual bool onEventEpollOut() override;

		// 转发端关闭套接字
		bool onEventForwardClose() { TCPNetworkItem::CloseSocket(_usRountID); };

		bool CloseSocket(ushort usRountID) override;


	private:
		WebSocketProtocol _websocketProtocol;

	protected:
	private:
	};


	// 转发服务器条目
	class TCPNetworkItemForward : public TCPNetworkItem
	{
		friend class TCPNetworkEngine;
		friend class TCPNetworkThreadForwardRW;

	public:
		TCPNetworkItemForward(ushort index, ITCPNetworkItemSink *pITCPNetworkItemSink) 
			:TCPNetworkItem(index, pITCPNetworkItemSink) {};

	public:
		virtual bool SendData(const void *pData, ushort wDataSize, ushort usRountID) override;

		// 接受数据
		virtual bool onEventEpollIn() override;

		// epollout
		virtual bool onEventEpollOut() override;

		bool onEventClientClose() { TCPNetworkItem::CloseSocket(_usRountID); };

		virtual unsigned attach(int sockfd, unsigned uClientIP) override;

		bool CloseSocket(ushort usRountID) override;

	};
};

#endif /* TCPNetworkEngine_hpp */
