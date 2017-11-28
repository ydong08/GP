//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//          佛曰:  
//                  写字楼里写字间，写字间里程序员；  
//                  程序人员写程序，又拿程序换酒钱。  
//                  酒醒只在网上坐，酒醉还来网下眠；  
//                  酒醉酒醒日复日，网上网下年复年。  
//                  但愿老死电脑间，不愿鞠躬老板前；  
//                  奔驰宝马贵者趣，公交自行程序员。  
//                  别人笑我忒疯癫，我笑自己命太贱；  
//                  不见满街漂亮妹，哪个归得程序员？  
//
//  TCPNetworkEngine.cpp
//  websocket_forward_prj
//
//  Created by 水深 on 16/11/15.
//  Copyright © 2016年 水深. All rights reserved.
//

#include "TCPNetworkEngine.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/epoll.h>
#include <algorithm>
#include <iostream>


using KernelEngine::TCPNetworkEngine;
using KernelEngine::TCPNetworkThreadAccept;
using KernelEngine::TCPNetworkThreadClientRW;
using KernelEngine::TCPNetworkItem;
using KernelEngine::TCPNetworkItemClient;
using KernelEngine::TCPNetworkItemForward;
using KernelEngine::WebSocketProtocol;
using KernelEngine::TCPNetworkThreadForwardRW;
using KernelEngine::TCPNetworkDataCache;
using KernelEngine::SendResult;

TCPNetworkEngine::~TCPNetworkEngine()
{
	std::cout << "TCPNetworkEngine::~TCPNetworkEngine()" << std::endl;
	closeServices();
}

TCPNetworkEngine::TCPNetworkEngine():_nPort(0)
{
    
}

/**
 * 设置参数
 */
bool TCPNetworkEngine::setServiceParameter(int nServicePort, size_t nMaxConnect, const std::string &serviceip, const std::string &forwardIp, int nForwardPort)
{
    _nPort = nServicePort;
    _nMaxConnect = nMaxConnect;
    
	_serviceip = serviceip;
	_forwardIp = forwardIp;
	_nForwardPort = nForwardPort;

    return true;
}


//读取配置的服务器端口
bool TCPNetworkEngine::readPortConfig(std::string confile)
{
	FILE* fp = fopen(confile.c_str(), "r+");
	if (NULL == fp)
	{
		std::cout << "open port config file failed" << std::endl;
		return false;
	}

	int nPort = 0;
	char line[80];
	char* ptr = line;
	memset(line, 0, sizeof(line));
	while (fgets(line, 80, fp) != NULL)
	{
		for (int i = 0; i < 80; i++){
			if (ptr[i] == '='){
				if (nPort < MAX_LISTEN_PORT){
					_nPortArray[nPort] = atoi(&ptr[i + 1]);
					nPort++;
				}
			}			
		}
		memset(line, 0, sizeof(line));
	}
	return true;
}


// 获取条目
TCPNetworkItemClient *TCPNetworkEngine::getNetworkItem(int index)
{
	
	assert(index < _vecNetworkItemStorage.size() && index >= 0);
	if (index > _vecNetworkItemStorage.size() || index < 0)
	{
		return nullptr;
	}
	_mutex.lock();
	auto pTCPNetworkItem = _vecNetworkItemStorage[index];
	_mutex.unlock();

	return (TCPNetworkItemClient *)pTCPNetworkItem;
}


/**
 * 发送数据
 */
bool TCPNetworkEngine::sendData(int32_t nSocketID, const void *pData, size_t nDataSize)
{

	auto pTCPNetworkItem = getNetworkItem(SOCKET_INDEX(nSocketID));

	if (pTCPNetworkItem == nullptr)
	{
		return false;
	}

    if (pTCPNetworkItem->SendData(pData, nDataSize, SOCKET_ROUNTID(nSocketID)) == false)
    {
        pTCPNetworkItem->CloseSocket(pTCPNetworkItem->_usRountID);
        return false;
    }
    
    return true;
}


/**
 * 开始服务
 */
bool TCPNetworkEngine::startServices()
{
	assert(false == _bService);
    
    if (NULL == INIT_MEMPORY_POLL(NETWORKIEM_MEMORY_SIZE))
    {
        std::cout << "创建缓存池失败" << std::endl;
        return false;
    };
	
	if (_bService)
	{
		return false;
	}

	//if (false == createServerSock()) // 创建套接字
	//{
	//	return false;
	//}
	// 创建套接字,同时监听多个游戏端口
	int sock = 0;
	for (int i = 0; i < MAX_LISTEN_PORT; i++)
	{
		if (_nPortArray[i] > 0)
		{
			if ((sock = createServerSock(_nPortArray[i])) != -1)
			{
				_nSockArray[i] = sock;
				sockport.insert(std::make_pair(_nSockArray[i], _nPortArray[i]));
			}
			else{
				return false;
			}
		}
	}

	if (false == createEpoll()) // 创建epoll
	{
		return false;
	}
    
	if (false == _TCPNeworkThreadAccept.InitThread(_nPortArray, _epoll_fd, this)) { return false; }
	if (false == _TCPNetworkThreadForwardRW.InitThread(_forward_epoll_fd)) { return false;  }
	if (false == _TCPNetworkThreadClientRW.InitThread(_client_epoll_fd)) { return false;  }

	_TCPNeworkThreadAccept.startThread();
	_TCPNetworkThreadForwardRW.startThread();
	_TCPNetworkThreadClientRW.startThread();

	_bService = true; 
    
	std::cout << "服务开启成功...." << std::endl;

	while (true)
	{
		sleep(100);
	}
    return true;
}


TCPNetworkItemForward *TCPNetworkEngine::findForwardItem(TCPNetworkItemClient *pClientItem)
{
	
	_mutex.lock();
	
	auto iter = find_if(_vecNetworkItemActive.begin(), _vecNetworkItemActive.end(),
		[=](ForwardStruct *&pForwardStruct){
		return pForwardStruct->pTCPNetworkItemClient == pClientItem;
	});
	
	_mutex.unlock();
	if (_vecNetworkItemActive.end() == iter)
	{
		return nullptr;
	}
   
	return (*iter)->pTCPNetworkItemForward;
}


TCPNetworkItemClient *TCPNetworkEngine::findClientItem(TCPNetworkItemForward *pForwardItem)
{
	_mutex.lock();
	auto iter = find_if(_vecNetworkItemActive.begin(), _vecNetworkItemActive.end(),
		[=](ForwardStruct *&pForwardStruct){
		return pForwardStruct->pTCPNetworkItemForward == pForwardItem;
	});

	_mutex.unlock();

	if (_vecNetworkItemActive.end() == iter)
	{
		return nullptr;
	}
	
	return (*iter)->pTCPNetworkItemClient;
}


bool TCPNetworkEngine::removeItemEvent(TCPNetworkItemClient *pTCPNetworkItemClient, TCPNetworkItemForward *pTCPNetworkItemForward)
{
	// 移除事件监听
    if (pTCPNetworkItemClient)
    {

        if (removeClientEpollEvent(pTCPNetworkItemClient->_socketClient))
        {
            std::cout << "移除客户端套接字监听成功" << std::endl;
        }
        else
        {
            std::cout << "移除客户端套接字监听失败" << std::endl;
        }

    }
    else
    {
        std::cout << "removeItemEvent pTCPNetworkItemClient is null";
    }
    
    if (pTCPNetworkItemForward)
    {

        if (removeForwardEpollEvent(pTCPNetworkItemForward->_socketClient))
        {
            std::cout << "移除转发端套接字监听成功" << std::endl;
        }
        else
        {
            std::cout << "移除转发端套接字监听失败" << std::endl;
        }
        
    }
    else
    {
        std::cout << "removeItemEvent pTCPNetworkItemClient is null";
    }
    
	return true;
}


// 客户端套接字关闭
bool TCPNetworkEngine::onEventClientShutdown(TCPNetworkItemClient *pTCPNetworkItem)
{
	std::cout << "onEventClientShutdown" << std::endl;
	_mutex.lock();
	auto iter = find_if(_vecNetworkItemActive.begin(), _vecNetworkItemActive.end(),
		[=](ForwardStruct *&pForwardStruct){
		return pForwardStruct->pTCPNetworkItemClient == pTCPNetworkItem;
	});

	if (_vecNetworkItemActive.end() == iter)
	{
        _mutex.unlock();
		removeItemEvent(pTCPNetworkItem, nullptr);
		std::cout << "onEventClientShutdown 没有找到转发端口" << std::endl;
		assert(false);
		return false;
	}
    
	// 加入到缓存中
	auto pForward = *iter;
	_vecNetworkItemActive.erase(iter);
	_vecNetworkItemBuffer.push_back(pForward);
	_mutex.unlock();
    
    // 移除事件监听
    removeItemEvent(pTCPNetworkItem, pForward->pTCPNetworkItemForward);

	// 转发端 关闭套接字
	pForward->pTCPNetworkItemForward->onEventClientClose();
    	
	std::cout << "onEventClientShutdown end" << std::endl;
	return true;
}

// 转发端套接字关闭
bool TCPNetworkEngine::onEventForwardShutdown(TCPNetworkItemForward *pTCPNetworkItem)
{
	std::cout << "onEventForwardShutdown" << std::endl;
	
	_mutex.lock();
	auto iter = find_if(_vecNetworkItemActive.begin(), _vecNetworkItemActive.end(),
		[=](ForwardStruct *&pForwardStruct){
		return pForwardStruct->pTCPNetworkItemForward == pTCPNetworkItem;
	});
    
	if (_vecNetworkItemActive.end() == iter)
	{
        _mutex.unlock();
		removeItemEvent(nullptr, pTCPNetworkItem);
		std::cout << "onEventForwardShutdown 没有找到转发端口" << std::endl;
		assert(false);
		return false;
	}

	// 加入到缓存中
	auto pForward = *iter;
	_vecNetworkItemActive.erase(iter);
	_vecNetworkItemBuffer.push_back(pForward);
	_mutex.unlock();
    
    // 移除事件监听
    removeItemEvent(pForward->pTCPNetworkItemClient, pTCPNetworkItem);

	// 转发端 关闭套接字
	pForward->pTCPNetworkItemClient->onEventForwardClose();
	
	return true;
}

/**
 * 关闭服务
 */
bool TCPNetworkEngine::closeServices()
{
	// 删除网络条目
	for (TCPNetworkItemVector::iterator iter = _vecNetworkItemActive.begin();
		iter != _vecNetworkItemActive.end(); ++iter) {
		(*iter)->pTCPNetworkItemClient->CloseSocket((*iter)->pTCPNetworkItemClient->_usRountID);
	}

	for (TCPNetworkItemVector::iterator iter = _vecNetworkItemStorage.begin();
		iter != _vecNetworkItemStorage.end(); ++iter) {
		delete (*iter)->pTCPNetworkItemClient;
		delete (*iter)->pTCPNetworkItemClient;
		delete *iter;
	}
    
    // 清空数据
    _vecNetworkItemStorage.clear();
    _vecNetworkItemBuffer.clear();
    _vecNetworkItemActive.clear();
    
	_TCPNeworkThreadAccept.stopThread();
	_TCPNetworkThreadForwardRW.stopThread();
	_TCPNetworkThreadClientRW.startThread();

	// 关闭套接字
	close(_serverSocket);
	close(_epoll_fd);
	close(_forward_epoll_fd);
	close(_client_epoll_fd);

    return true;
}

/**
 * 关闭客户端套接字
 */
bool TCPNetworkEngine::CloseSocket(int nSocketID)
{
   
    return true;
}

/**
 * 获取当前端口号
 */
int TCPNetworkEngine::getCurrentPort()
{
    return _nPort;
}


/**
 * 创建服务端套接字
 */
int TCPNetworkEngine::createServerSock(int port)
{
    // 创建套接字
    int sockfd_server = socket(AF_INET, SOCK_STREAM, 0);
    
    if (-1 == sockfd_server)
    {
        return -1;
    }
    
    struct sockaddr_in _sockaddr;
    memset(&_sockaddr, 0, sizeof(_sockaddr));
    _sockaddr.sin_family = AF_INET;
    _sockaddr.sin_port = htons(port);
    _sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	int ret = setsockopt(sockfd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    // 绑定套接字
    if (-1 == bind(sockfd_server, (struct sockaddr *)&_sockaddr, sizeof(struct sockaddr_in)))
    {
        // 绑定套接字失败
		std::cout << "绑定套接字失败" << std::endl;
        return -1;
    }
    
    // 监听
    if (listen(sockfd_server, nMaxConnect))
    {
		std::cout << "监听套接字失败" << std::endl;
        return -1;
    }
 
    //_serverSocket = sockfd_server;

  //  return true;
	return sockfd_server;
}

/**
 * 创建和注册epoll
 */
bool TCPNetworkEngine::createEpoll()
{
	// 创建epoll
	_epoll_fd = epoll_create(nEpollCreateMax);
	_client_epoll_fd = epoll_create(nEpollCreateMax);
	_forward_epoll_fd = epoll_create(nEpollCreateMax);

	assert(_epoll_fd != -1 && _client_epoll_fd != -1 && _forward_epoll_fd != -1);

	if (_epoll_fd == -1 || _client_epoll_fd == -1 || _forward_epoll_fd == -1)
	{
		return false;
	}

	int sock = 0;
	for (int i = 0; i < MAX_LISTEN_PORT; i++)
	{
		if (_nSockArray[i] > 0)
		{
			if (false == registerEpoll(_epoll_fd, _nSockArray[i]))
			{
				return false;
			}
		}
	}

	return true;
}

bool TCPNetworkEngine::registerEpoll(int epoll_fd, int sockfd)
{
    epoll_event event;
    event.data.ptr = (void *)this;
    event.events = EPOLLIN;
    
    // 注册服务端epoll事件
	if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event))
    {
        return false;
    }
    
    return true;
}

/************************************************************************/
/* 移除事件监听                                                         */
/************************************************************************/
bool TCPNetworkEngine::removePollEvent(int epoll_fd, int sockfd)
{
	// 注册服务端epoll事件
	std::cout << "removePollEvent: epool_fd = " << epoll_fd << ", sockfd=  " << sockfd;
	epoll_event ev;
	if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockfd, &ev))
	{
		std::cout << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

/************************************************************************/
/* 注册客户端事件监听                                                   */
/************************************************************************/
bool TCPNetworkEngine::registerEpollClient(int epoll_fd, TCPNetworkItem *pNetworkItem)
{
	epoll_event event;
	event.data.ptr = (void *)pNetworkItem;
	event.events = EPOLLIN | EPOLLET;

	// 注册服务端epoll事件
	if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pNetworkItem->_socketClient, &event))
	{
        std::cout << "registerEpollClient failed" << std::endl;
		return false;
	}

	return true;

}

/**
* 激活一个网络条目
*/
TCPNetworkItem *TCPNetworkEngine::activeNetworkItem()
{
	_mutex.lock();
	TCPNetworkItem * pNetworkItem = nullptr;
	// 查找空闲条目
	for (TCPNetworkItemVector::iterator iter = _vecNetworkItemBuffer.begin();
		iter != _vecNetworkItemBuffer.end(); ++iter) {

		ForwardStruct *pForward = *iter;
		_vecNetworkItemActive.push_back(pForward);
		_vecNetworkItemBuffer.erase(iter);

		pNetworkItem = pForward->pTCPNetworkItemClient;
		
		break;
	}
    
    _mutex.unlock();
	
	if (nullptr != pNetworkItem) {
		std::cout << "从缓存中获取对象成功" << std::endl;
		return pNetworkItem;
	}
    
    _mutex.lock();
	
	size_t nCurrentConnect = _vecNetworkItemStorage.size();
    _mutex.unlock();
    
	if (nCurrentConnect > nMaxConnect) {
		return nullptr;
	}

	_mutex.lock();
	auto pTCPNetworkItemClient = new TCPNetworkItemClient(nCurrentConnect, this);
	auto pTCPNetworkItemForward = new TCPNetworkItemForward(nCurrentConnect, this);

    std::shared_ptr<std::mutex> forwardMutex(new std::mutex);
	ForwardStruct *pForwardStruct = new ForwardStruct();
    
    pTCPNetworkItemClient->_mutex = forwardMutex;
    pTCPNetworkItemForward->_mutex = forwardMutex;
	pForwardStruct->pTCPNetworkItemClient = pTCPNetworkItemClient;


	pForwardStruct->pTCPNetworkItemForward = pTCPNetworkItemForward;


	_vecNetworkItemActive.push_back(pForwardStruct);
	_vecNetworkItemStorage.push_back(pForwardStruct);
	_mutex.unlock();
	return pTCPNetworkItemClient;
}



// 客户端连接成功
bool TCPNetworkEngine::onEventSocketBind(TCPNetworkItemClient *pTCPNetworkItem)
{
	std::cout << "绑定客户端套接字" << std::endl;
	assert(pTCPNetworkItem != nullptr);

	if (pTCPNetworkItem == nullptr)
	{
		return false;
	}

	// 连接转发服务器 

	int _forwardFd = socket(AF_INET, SOCK_STREAM, 0);

	if (-1 == _forwardFd)
	{
		return false;
	}


	sockaddr_in _sockaddr;

	memset(&_sockaddr, 0, sizeof(sockaddr_in));

	_sockaddr.sin_port = htons(_nForwardPort);
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_addr.s_addr = inet_addr(_forwardIp.c_str());

//
//	std::thread thread([=]{
		if (-1 == connect(_forwardFd, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)))
		{
			// 连接错误
			std::cout << "连接转发服务器错误!!!" << std::endl;
            close(_forwardFd);
			pTCPNetworkItem->onEventForwardBindFailure();
			pTCPNetworkItem->onEventForwardClose();
		}
		else
		{
			int flags = fcntl(_forwardFd, F_GETFL);
			decltype(findForwardItem(pTCPNetworkItem)) pForwardItem = nullptr;
			if (-1 == fcntl(_forwardFd, F_SETFL, flags | O_NONBLOCK))
			{
				std::cout << "设置服务端套接字属性失败" << std::endl;

				close(_forwardFd);

				// 设置套接字属性失败
				pTCPNetworkItem->onEventForwardClose();

                // 绑定失败
                pTCPNetworkItem->onEventForwardBindFailure();

				return false;
			}

			std::cout << "转发端套接字: " << _forwardFd << std::endl;
            pTCPNetworkItem->forward_lock();
			bool bRet = true;
			do
			{
				// 获取对应的转发服务器item
				pForwardItem = findForwardItem(pTCPNetworkItem);

				if (pForwardItem == nullptr)
				{
					
					close(_forwardFd);
                    
                    // 设置套接字属性失败
                    pTCPNetworkItem->onEventForwardClose();
                    
                    // 绑定失败
                    pTCPNetworkItem->onEventForwardBindFailure();
                    
                    return false;
				}
                
				pForwardItem->attach(_forwardFd, 127001);
				bRet = registerEpollClient(_client_epoll_fd, pTCPNetworkItem);
				if (false == bRet)
				{
					std::cout << "注册客户端读写失败" << std::endl;
					break;
				}

				bRet = registerEpollClient(_forward_epoll_fd, pForwardItem);

				if (false == bRet)
				{
					std::cout << "注册转发端读写失败" << std::endl;
					break;
				}



			} while (0);

			if (bRet)
			{
				// 连接成功
				pTCPNetworkItem->onEventForwardBindSuccess();
			}
			else
			{
				// 关闭套接字
				pTCPNetworkItem->CloseSocket(pTCPNetworkItem->_usRountID);
				// 绑定失败
				pTCPNetworkItem->onEventForwardBindFailure();

			}
            pTCPNetworkItem->forward_unlock();

		}
//	});
//
//	thread.detach();

	return true;
}

// 客户端读入数据
bool TCPNetworkEngine::onEventSocketRead(TCPNetworkItemClient *pTCPNetworkItem, void *pData, ushort usDataSize)
{
	// 转发数据

	TCPNetworkItemForward *pTCPNetworkItemForward = findForwardItem(pTCPNetworkItem);

	if (pTCPNetworkItemForward == nullptr) { 
		// 没有对应的转发服务器条目
		std::cout << "没有对应的转发服务器条目" << std::endl;
		
		return false;
	}

	return pTCPNetworkItemForward->SendData(pData, usDataSize, pTCPNetworkItem->getIndex());
}


// 转发端读入服务器数据并发送到客户端
bool TCPNetworkEngine::onEventForwardSocketRead(TCPNetworkItemForward *pTCPNetworkItem, void *pData, ushort usDataSize)
{
	//sendData(1, pData, usDataSize);
	std::cout << " 发送数据到客户端" << std::endl;
	auto pTCPNetworkItemClient = findClientItem(pTCPNetworkItem);

	if (pTCPNetworkItemClient == nullptr) {
		// 没有对应的客户端条目
		std::cout << " 没有对应的客户端条目" << std::endl;
		return false;
	}

	return pTCPNetworkItemClient->SendData(pData, usDataSize, pTCPNetworkItemClient->_usRountID);

}


bool TCPNetworkEngine::removeClientEpollEvent(int sockfd) // 删除客户端epoll 监听
{
	return removePollEvent(_client_epoll_fd, sockfd);
}

bool TCPNetworkEngine::removeForwardEpollEvent(int sockfd) // 删除转发端epoll 监听
{
	return removePollEvent(_forward_epoll_fd, sockfd);
}


bool TCPNetworkEngine::modofiClientEvent(TCPNetworkItem *item, int events)
{
    struct epoll_event ev;
    ev.data.ptr = (void *)item;
    ev.events = events;
    
    if (epoll_ctl(_client_epoll_fd, EPOLL_CTL_MOD, item->_socketClient, &ev) == -1)
        return false;
    
    return true;
}


bool TCPNetworkEngine::modifiForwardEvent(TCPNetworkItem *item, int events)
{
    struct epoll_event ev;
    ev.data.ptr = (void *)item;
    ev.events = events;
    
    if (epoll_ctl(_forward_epoll_fd, EPOLL_CTL_MOD, item->_socketClient, &ev) == -1)
        return false;
    
    return true;
}

////////////////////////////////////// 连接线程 //////////////////////////////////////////////
bool TCPNetworkThreadAccept::InitThread(int socket_fd[], int epoll_fd, TCPNetworkEngine *pNetworkEngine)
{
    assert(epoll_fd != -1);
	assert(socket_fd != nullptr);
    assert(pNetworkEngine != nullptr);
    
	if (epoll_fd == -1 || socket_fd == nullptr)
    {
        return false;
    }
    
    if (pNetworkEngine == nullptr)
    {
        return false;
    }
    
    _epoll_fd = epoll_fd;
    _pNetworkEngine = pNetworkEngine;
	for (int i = 0; i < MAX_LISTEN_PORT; i++)
	{
		_socket_fd[i] = socket_fd[i];
	}
    
    return true;
}

bool TCPNetworkThreadAccept::onEventThreadRunOne(int sockfd)
{
	// 清空数据
	memset(&events, 0, sizeof(events));
	int nfds = epoll_wait(_epoll_fd, events, kMaxEvents, -1);
	for (int i = 0; i < nfds; ++i) {

		// 错误判断
		if(events[i].events & (EPOLLHUP | EPOLLERR))
		{
			continue;
		}

		if (events[i].events & EPOLLIN)
		{
			std::cout << "客户端连接" << std::endl;
			sockaddr_in socketAddr;
			socklen_t len = sizeof(sockaddr_in);
			int clientSocketFd = accept(_socket_fd, (sockaddr *)&socketAddr, &len);

			if (-1 == clientSocketFd) {
				std::cout << "socketfd: %d " << _socket_fd << " connect on port: %d " <<  (TCPNetworkEngine::sockport).find(_socket_fd).second
					<< "客户端连接失败: " << strerror(errno) << std::endl;
				continue;
			}

			int flags = fcntl(clientSocketFd, F_GETFL);
			if (-1 == fcntl(clientSocketFd, F_SETFL, flags | O_NONBLOCK))
			{
				std::cout << "设置客户端套接字属性失败" << std::endl;
				// 设置套接字属性失败
				return false;
			}

			TCPNetworkItem *pTCPNetworkItem = _pNetworkEngine->activeNetworkItem();

			if (pTCPNetworkItem == nullptr) {
				std::cout << "激活网络条目失败" << std::endl;
				close(clientSocketFd);
				continue;
			}

			pTCPNetworkItem->attach(clientSocketFd, socketAddr.sin_addr.s_addr);
		} 
	}

	return true;

}

bool TCPNetworkThreadAccept::onEventThreadRun()
{
    for(int i = 0; i < MAX_LISTEN_PORT; i++)
	{
		if(_socket_fd[i] > 0)
		{
			if(! onEventThreadRunOne(_socket_fd[i]))
				return false;
		}	
	}
    
    return true;
    
}


void TCPNetworkThreadAccept::onEventThreadStart()
{
	std::cout << "连接线程开始运行" << std::endl;
}



////////////////////////////////////// 读写线程 //////////////////////////////////////////////
bool TCPNetworkThreadClientRW::InitThread(int epoll_fd)
{
	assert(epoll_fd != -1);

	if (epoll_fd == -1)
	{
		return false;
	}


	_epoll_fd = epoll_fd;

	return true;
}


bool TCPNetworkThreadClientRW::onEventThreadRun()
{
	// 清空数据
	memset(&events, 0, sizeof(events));
	int nfds = epoll_wait(_epoll_fd, events, kMaxEvents, -1);
    
	TCPNetworkItem *pTCPNetworkItem = nullptr;

	for (int i = 0; i < nfds; ++i) {
		
		pTCPNetworkItem = (TCPNetworkItem *)(events[i].data.ptr);
		
		assert(pTCPNetworkItem);

		if (pTCPNetworkItem == nullptr)
		{
			continue;
		}
		
		// 错误判断
		if (events[i].events & (EPOLLHUP | EPOLLERR))
		{
            pTCPNetworkItem->forward_lock();
			if (events[i].events & EPOLLHUP) std::cout << "EPOLLHUP" << std::endl;
			if (events[i].events & EPOLLERR) std::cout << "EPOLLERR" << std::endl;
			pTCPNetworkItem->CloseSocket(pTCPNetworkItem->_usRountID);
			std::cout << strerror(errno) << std::endl;;
			std::cout << "接受数据发生错误" << std::endl;
            
            pTCPNetworkItem->forward_unlock();
			continue;
		}
		else if (events[i].events & EPOLLIN)
		{
			std::cout << "接受数据成功" << std::endl;
			// 如果读取成功
			pTCPNetworkItem->forward_lock();
			pTCPNetworkItem->onEventEpollIn();
			pTCPNetworkItem->forward_unlock();
			continue;
		}
		else if (events[i].events & EPOLLOUT)
		{
			pTCPNetworkItem->forward_lock();
			if (pTCPNetworkItem->onEventEpollOut() == false)
            {
                pTCPNetworkItem->CloseSocket(pTCPNetworkItem->_usRountID);
            }
			pTCPNetworkItem->forward_unlock();
			continue;
		}
		else {
			
		}

		

	}

	return true;

}


void TCPNetworkThreadClientRW::onEventThreadStart()
{
	std::cout << "读写线程正在运行" << std::endl;
	return ;
}

////////////////////////////////// 客户端连接条目 /////////////////////////////////

// 构造函数
TCPNetworkItem::TCPNetworkItem(ushort index, ITCPNetworkItemSink *pITCPNetworkItemSink)
{
    assert(index >= 0);
    assert(pITCPNetworkItemSink != nullptr);
    _usIndex = index;
    _pITCPNetworkItemSink = pITCPNetworkItemSink;
}

TCPNetworkItem::~TCPNetworkItem()
{
	close(_socketClient);
}

// 关闭套接字
bool TCPNetworkItem::CloseSocket(ushort usRountID)
{
    std::cout << "CloseSocket --------->>>>>>>> " << std::endl;
	assert(_usRountID == usRountID);

	if ((_usRountID != usRountID))
	{
		return false;
	}

	_bClose = true;
	close(_socketClient);

	resumeData();
    return true;
}

// 重置数据
unsigned TCPNetworkItem::resumeData()
{
	
	_activeTime = 0;
	_socketClient = -1;
	_usRountID = _usRountID + 1;

	_bSendIng = false;
	_bRecvIng = false;
	_bShutDown = false;
	
	
	_usRecvSize = 0;
	memset(_cbRecvBuf, 0, sizeof(_cbRecvBuf));

	_sendTickCount = 0;
	_recvPacketCount = 0;
	_recvTickCount = 0;
    _dataCache.resetData();
    return 10;
}

// 绑定客户端对象
unsigned TCPNetworkItem::attach(int sockfd, unsigned uClientIP)
{
    assert(sockfd != -1);
    assert(uClientIP != 0);
    
    _socketClient = sockfd;
    _uClientIP = uClientIP;
    
    // 状态变量
    _bSendIng = false;
    _bRecvIng = false;
    _bShutDown = false;
    
    _activeTime = time(NULL);
    _recvTickCount = time(NULL);
    
	_bClose = false;
    // _pITCPNetworkItemSink->onEventSocketBind(this);
    // 调用回调钩子
    return getIdentifierID();
}


// 发送数据
SendResult TCPNetworkItem::_sendData(const void *pData, ushort wDataSize)
{
	assert(nullptr != pData);

	if (pData == nullptr) {
		std::cout << "pData == nullptr" << std::endl;
		return SEND_ERROR;
	}
    
    int size = 0;
    if (_dataCache.getBufferLen() == 0)
    {
		std::cout << "_dataCache.getBufferLen() == 0" << std::endl;
		size = send(_socketClient, pData, wDataSize, 0);
        
        if (size == -1) /********** 没完成 ***********/
        {
            if (errno == EINTR || errno == EAGAIN || errno == EINPROGRESS)
            {
                // 缓冲区 满了 进行处理
                std::cout << "缓冲区 满了 进行处理" << std::endl;
                if (false == _dataCache.addDataBuffer(pData, wDataSize))
                {
                    return SEND_ERROR;
                }
                return SEND_FULL;
            }
            else
            {
                std::cout << "发送数据错误" << std::endl;
                return SEND_ERROR;
            }
        }
    }
    else
    {
        // 数据添加到缓存
        if (false == _dataCache.addDataBuffer(pData, wDataSize))
        {
            return SEND_ERROR;
        }
    }
	
	std::cout << "发送数据成功: 大小 ====>>>" << size << std::endl;
	return SEND_SUCCESS;
}


bool TCPNetworkItem::onEventEpollOut()
{
    if (_dataCache.getBufferLen()) {
        int ret = send(_socketClient, _dataCache.getDataBuffer(), _dataCache.getBufferLen(), 0);
        
        if (ret == -1) {
            if(errno != EINTR && errno != EAGAIN && errno != EINPROGRESS)
            {
                return false;
            }
        }
        
        if (ret == _dataCache.getBufferLen()) { // 数据发送完成 修改epoll事件为 epollin
            _dataCache.freeDataBuffer();
        }
        else if (ret < _dataCache.getBufferLen())
        {
            _dataCache.skipDataBuffer(ret);
        }
        
        return true;
    }
    
    return true;
}


bool TCPNetworkItemClient::onEventForwardBindSuccess()
{
	// 连接转发服务器成功之后 添加到读写线程当中
	std::cout << "绑定转发服务器成功" << std::endl;
	return true;
}


bool TCPNetworkItemClient::onEventForwardBindFailure()
{
    std::cout << "绑定转发服务器失败" << std::endl;
	return true;
}


// 绑定客户端对象
unsigned TCPNetworkItemClient::attach(int sockfd, unsigned uClientIP)
{
	TCPNetworkItem::attach(sockfd, uClientIP);

	_pITCPNetworkItemSink->onEventSocketBind(this);
	// 调用回调钩子
	return getIdentifierID();
}

// 转发端将数据发送到客户端
bool TCPNetworkItemClient::SendData(const void *pData, ushort wDataSize, ushort usRountID)
{
	std::cout << " 发送数据到客户端: enter SendData" << std::endl;
	char sendBuf[SOCKET_TCP_BUFFER];
	int size = sizeof(sendBuf);
	if (false == _websocketProtocol.encodeData(pData, wDataSize, sendBuf, &size))
	{
		return false;
	}
    SendResult _result = _sendData(sendBuf, size);
    switch (_result) {
        case SEND_SUCCESS:
            return true;
            
        case SEND_FULL:
            return this->_pITCPNetworkItemSink->modofiClientEvent(this, EPOLLIN | EPOLLET | EPOLLOUT);
            
        default:
            return false;
    }
    
    return false;
}

// 接受数据
bool TCPNetworkItemClient::onEventEpollIn()
{
	do
	{
		time_t tm1 = time(NULL);
		int recvSize = recv(_socketClient, _cbR	ecvBuf + _usRecvSize, sizeof(_cbRecvBuf) - _usRecvSize, 0);

		std::cout << "接收数据的大小为: " << recvSize << ", 当前套接字: " << _socketClient << std::endl;


		if (0 == recvSize) // 客户端关闭套接字
		{
			std::cout << "客户端关闭套接字" << std::endl;
			CloseSocket(_usRountID);
			return false;
		}

		if (SOCKET_ERROR == recvSize) // 发生错误
		{
			if (EAGAIN == errno || EINTR == errno || EWOULDBLOCK == errno)
			{
				std::cout << "EAGAIN || EINTER || EWOULDBLOCK" << std::endl;
				break;
			}
			else
			{
				std::cout << "发生错误" << std::endl;
				CloseSocket(_usRountID);
				return false;
			}
		}


		_usRecvSize += recvSize;

		std::cout << "当前大小: " << _usRecvSize << std::endl;


	} while(true);
    
	if (0 == _recvPacketCount) // 第一次接受客户端数据
	{
		// 读取客户端数据
		ushort trueSize = 0;

		// 检查协议接受是否完成
		if (false == _websocketProtocol.checkWebsocketRecvFinish(_cbRecvBuf, _usRecvSize, &trueSize)) 
		{
			return true;
		}

		// 解析头
		if (false == _websocketProtocol.parseWebsocketHead(_cbRecvBuf, trueSize))
		{
			// 解析头失败 关闭套接字
			std::cout << "解析头失败 关闭套接字" << std::endl;
			CloseSocket(_usRountID);
			return false;
		}


		// 解析成功 获取握手数据
		int size = 0;

		std::cout << "解析成功 获取握手数据" << std::endl;
		const char *pData = _websocketProtocol.getSendBrowseKeyData(&size);

		if (pData == nullptr)
		{
			CloseSocket(_usRountID);
			return false;
		}

		SendResult bSend = _sendData(pData, size);

		if (bSend == SEND_ERROR)
		{
			CloseSocket(_usRountID);
			return false;
		}
        else if (bSend == SEND_FULL &&
                _pITCPNetworkItemSink->modofiClientEvent(this, EPOLLIN | EPOLLOUT | EPOLLET) == false)
        {
            CloseSocket(_usRountID);
            return false;
        }

		_usRecvSize -= trueSize;


		memmove(_cbRecvBuf, _cbRecvBuf + trueSize, _usRecvSize);

		_recvPacketCount++;
		return true;
	}

	//接收完成
	u_char cbBuffer[SOCKET_TCP_BUFFER] = { 0 };

	while (_usRecvSize >= _websocketProtocol.getPacketHeadSize())
	{
		int packetsize = _websocketProtocol.getPacketSize(_cbRecvBuf, _usRecvSize);
		std::cout << "解包大小为: " << packetsize << std::endl;
		if (packetsize == -1)
		{
			// 获取数据大小错误
			CloseSocket(_usRountID);
			return false;
		}

		if (packetsize <= 0) // 无法计算出大小
		{
			break;
		}

		if (packetsize > SOCKET_TCP_BUFFER) // 数据包太长
		{
			CloseSocket(_usRountID);
			return false;
		}

		if (packetsize < _websocketProtocol.getPacketHeadSize()) // 数据包太短
		{
			CloseSocket(_usRountID);
			return false;
		}

		// 数据包接受不完整
		if (packetsize > _usRecvSize) {

			return true;
		}


		// 解析数据
		int buflen = sizeof(cbBuffer);
		if (_websocketProtocol.decodeData(_cbRecvBuf, packetsize, cbBuffer, &buflen) == false)
		{
			CloseSocket(_usRountID);
			return false;
		}

		// 回调处理转发
		if (_pITCPNetworkItemSink->onEventSocketRead(this, cbBuffer, buflen) == false) {
			CloseSocket(_usRountID);
			return false;
		}

		_usRecvSize -= packetsize;
		memmove(_cbRecvBuf, _cbRecvBuf + packetsize, _usRecvSize);
		_recvPacketCount++;

	}
	
	return true;
}


// 关闭套接字
bool TCPNetworkItemClient::CloseSocket(ushort usRountID)
{
	if (_bClose)
	{
        std::cout << "重复关闭套接字" << std::endl;
		return true;
	}
	_pITCPNetworkItemSink->onEventClientShutdown(this);
	// 关闭套接字
	TCPNetworkItem::CloseSocket(usRountID);
	
	return true;
}

// 接受数据
bool TCPNetworkItemClient::onEventEpollOut()
{
	std::cout << "TCPNetworkItemClient::onEventEpollOut" << std::endl; 
    bool bStatus = TCPNetworkItem::onEventEpollOut();
	// 数据发送完成
	if (bStatus && _dataCache.getBufferLen() == 0)
	{
		bStatus = _pITCPNetworkItemSink->modofiClientEvent(this, EPOLLIN | EPOLLET);
	}
	else // 没有发送完成 因为是边缘触发 EPOLLOUT 只会触发一次 所以 要重新添加
	{

		if (bStatus)
		{
			bStatus = _pITCPNetworkItemSink->modofiClientEvent(this, EPOLLIN | EPOLLOUT | EPOLLET);
		}
	}

	return bStatus;
}


bool TCPNetworkItemForward::SendData(const void *pData, ushort wDataSize, ushort iIndex)
{
	std::cout << "转发端发送数据" << std::endl;
    SendResult _result = _sendData(pData, wDataSize);
    
    switch (_result) {
        case SEND_SUCCESS:
            return true;
        
        case SEND_FULL:
            return _pITCPNetworkItemSink->modifiForwardEvent(this, EPOLLIN | EPOLLOUT | EPOLLET);
            
        default:
            return false;
    }
    
    return false;
}


// 转发端接收数据-->将数据转发到客户端
bool TCPNetworkItemForward::onEventEpollIn()
{
	std::cout << "接收到服务器数据..." << std::endl;
	do
	{
		int tmpSize = recv(_socketClient, _cbRecvBuf + _usRecvSize, sizeof(_cbRecvBuf)-_usRecvSize, 0);

		std::cout << "接收数据的大小为: " << tmpSize << std::endl;


		if (0 == tmpSize) // 转发端关闭套接字
		{
			std::cout << "服务器已关闭, 转发端亦将关闭套接字" << std::endl;
			CloseSocket(_usRountID);
			return false;
		}

		if (SOCKET_ERROR == tmpSize) // 发生错误
		{
			if (EAGAIN == errno || EINTR == errno || EWOULDBLOCK == errno)
			{
				std::cout << "EAGAIN || EINTER || EWOULDBLOCK" << std::endl;
				break;
			}
			else
			{
				std::cout << "socket发生错误" << std::endl;
				CloseSocket(_usRountID);
				return false;
			}
		}

		_usRecvSize += tmpSize;
		
	} while(true);
	    
    if (_usRecvSize < 2) {
        return true;
    }
    
    size_t packetsize = ntohs(*((unsigned short*)_cbRecvBuf)) + 2; // 一个包的大小
	printf("1_usRecvSize: %d, packetsize: %d\n", _usRecvSize, packetsize);
    while (_usRecvSize >= packetsize) { // 一个包的大小
		printf("_usRecvSize: %d, packetsize: %d\n", _usRecvSize, packetsize);
        if (packetsize <= 0) // 无法计算出大小
        {
            CloseSocket(_usRountID);
            return false;
        }
        
        if (packetsize > SOCKET_TCP_BUFFER) // 数据包太长
        {
            CloseSocket(_usRountID);
            return false;
        }
        
        //转发端将数据发送到客户端
        if (_pITCPNetworkItemSink->onEventForwardSocketRead(this, _cbRecvBuf, packetsize) == false)
        {
            CloseSocket(_usRountID);
            return false;
        }
        _usRecvSize -= packetsize;
        _recvPacketCount++;
        memmove(_cbRecvBuf, _cbRecvBuf + packetsize, _usRecvSize);
        
		if (_usRecvSize > 2) 
		{
			packetsize = ntohs(*((unsigned short*)_cbRecvBuf)) + 2;
		} 
		else
		{
			break;
		}
    }
    
	
	return true;
}


// 接受数据
bool TCPNetworkItemForward::onEventEpollOut()
{
    bool bStatus = TCPNetworkItem::onEventEpollOut();
	// 数据发送完成
	if (bStatus && _dataCache.getBufferLen() == 0)
	{
		bStatus = _pITCPNetworkItemSink->modifiForwardEvent(this, EPOLLIN | EPOLLET);
	}
	else // 没有发送完成 因为是边缘触发 EPOLLOUT 只会触发一次 所以 要重新添加
	{
		if (bStatus)
		{
			bStatus = _pITCPNetworkItemSink->modifiForwardEvent(this, EPOLLIN | EPOLLOUT | EPOLLET);
		}
	}

	return bStatus;
}


unsigned TCPNetworkItemForward::attach(int sockfd, unsigned uClientIP)
{
	return TCPNetworkItem::attach(sockfd, uClientIP);
}


// 关闭套接字
bool TCPNetworkItemForward::CloseSocket(ushort usRountID)
{
	if (_bClose)
	{
		return true;
	}
	std::cout << "转发端关闭套接字" << std::endl;
	_pITCPNetworkItemSink->onEventForwardShutdown(this);
	TCPNetworkItem::CloseSocket(usRountID);
	
	return true;
}

////////////////////////////////////////////////////////////////////////// websocket 协议层


// 第一次解析头
bool WebSocketProtocol::parseWebsocketHead(void *pData, ushort wDataSize)
{
	_headInfoMap.clear();
	int lastIndex = 0;
	char *_pdata = (char *)pData;
	bool bGetKey = true;

	char keybuf[256] = { 0 };
	char valbuf[256] = { 0 };

	// std::cout << std::string(_pdata) << std::endl;
	for (ushort i = 0; i < wDataSize; ++i)
	{
		if (_pdata[i] == ':' && _pdata[i + 1] == ' '  && bGetKey)
		{
			
			memset(keybuf, 0, sizeof(keybuf));
			memcpy(keybuf, _pdata + lastIndex, i - lastIndex);
			lastIndex = i + 2; // 跳过:和空格

			bGetKey = false;

			continue;
		}
		else if ((_pdata[i] == '\r' || _pdata[i] == '\n')) // 有可能是 \r\n 也有可能是\n
		{
			if (bGetKey == false)
			{
				memset(valbuf, 0, sizeof(valbuf));
				memcpy(valbuf, _pdata + lastIndex, i - lastIndex);

				if (_pdata[i] == '\r') // 跳过回车
				{
					++i;
				}

				bGetKey = true;
				// std::cout << std::string(keybuf) << " ===> " << std::string(valbuf) << std::endl;
				_headInfoMap.insert(make_pair(std::string(keybuf), std::string(valbuf)));
			}
			else {
				if (_pdata[i] == '\r') // 跳过回车
				{
					++i;
				}
			}
			

			lastIndex = i + 1;

			
		}
	}


	auto iter = _headInfoMap.find("Sec-WebSocket-Key");
	
	if (iter == _headInfoMap.end())
	{
		return false;
	}
    
    std::cout << "Sec-WebSocket-Key: " << iter->second << std::endl;

	return true;
}

// 检测websocket协议是否接受完成
bool WebSocketProtocol::checkWebsocketRecvFinish(void *pData, ushort wDataSize, ushort *endSize)
{
	int enterCount = 0; // 回车数量统计

	if (!pData || !wDataSize)
		return false;

	u_char *_pData = (u_char *)pData;
	for (ushort i = 0; i < wDataSize; ++i)
	{
		char c = _pData[i];
	

		if (c == '\r') // 换行直接跳过
		{
			continue;
		}

		if (c == '\n')
		{
			enterCount++;
		}
		else
		{
			// 不是回车
			enterCount = 0;
		}

		if (enterCount == 2)
		{
			*endSize = i + 1;
			return true;
		}
	}

	return false;
}


const char *WebSocketProtocol::getSendBrowseKeyData(int *size)
{
	auto iter = _headInfoMap.find("Sec-WebSocket-Key");

	if (iter == _headInfoMap.end())
	{
		return nullptr;
	}

	char sha1Str[41] = { 0 };
	char base64str[64] = { 0 };

	std::string key = iter->second;
	key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::cout << key << std::endl;

	SHA1 *sha1 = new SHA1();
	sha1->addBytes(key.c_str(), key.length());
	unsigned char *digest = sha1->getDigest();

	Base64_Encode(base64str, reinterpret_cast<const char*>(digest), 20);


	delete sha1;
	free(digest);
	sendHead = "";
	sendHead += "HTTP/1.1 101 Switching Protocols\r\n";
	sendHead += "Connection: upgrade\r\n";
	sendHead += "Sec-WebSocket-Accept: ";
	sendHead += base64str;
	sendHead += "\r\n";
	sendHead += "Upgrade: websocket\r\n\r\n";

	*size = sendHead.length();

	std::cout << sendHead << std::endl;
	return sendHead.c_str();
}

// 检测包是否接受完成
bool WebSocketProtocol::checkPacketFinish(void *pData, ushort wDataSize)
{

}


int WebSocketProtocol::getPacketTrueHeadSize(char fmask)
{
	
	int iPackHeadSize = getPacketHeadSize(); // 包头的大小

	int len = fmask & ((1 << 7) ^ 0xff);
	bool bMask = fmask & (1 << 7);

	// 长度的偏移
	int offset = (len == 126) ? 2 : (len == 127) ? 8 : 0;

	iPackHeadSize += offset;

	if (bMask) {
		iPackHeadSize += 4;
	}

	return iPackHeadSize;
}

// 获取包的大小
int WebSocketProtocol::getPacketSize(void *pData, ushort wDataSize)
{
	assert(pData != nullptr);
	assert(wDataSize >= 2);

	int size = 0;

	if (pData == nullptr || wDataSize < 2)
	{
		return 0;
	}

	char *_pdata = (char *)pData;

	// 取后面7位的值
	char c = _pdata[1] & ((1 << 7) ^ 0xff);

	if (c < 126) // 小于126 就是长度
	{
		return c + getPacketTrueHeadSize(_pdata[1]);
	}

	switch (c)
	{
	case 126:
	{
		if (wDataSize < 4)
		{
			// 包大小不够
			return -2;
		}
		
		ushort *_shortPointer = (ushort *)_pdata;
		size = ntohs(_shortPointer[1]); // 获取大小
		std::cout << "Websocket short =====>" << size << std::endl;
		break;
	}

	case 127:
	{
		if (wDataSize < 10)
		{
			// 包大小不够
			return -2;
		}
		unsigned long long *_shortPointer = (unsigned long long *)(_pdata + 2);
		size = NTOHLL(_shortPointer[0]); // 获取大小
		std::cout << "Websocket longlong ======> " << size << std::endl;
		break;
	}

	default:
	{
		assert(false); // 肯定是不对的
		return -1;
	}
	}
	size += getPacketTrueHeadSize(_pdata[1]);
	return size;


}

int WebSocketProtocol::getPacketHeadSize()
{
	return WebsocketHeadSize;
}

/**
* @param in pData 未解包数据包
* @param in size  数据包大小
* @param out pNewData 缓冲区数据
* @param inout pNewDataSize 缓冲区数据大小 会返回数据真实大小
* @param 成功返回true 失败返回false
*/
bool WebSocketProtocol::decodeData(const void *pData, int size, void *pNewData, int *pNewDataSize)
{
	assert(pData != nullptr);
	assert(pNewData != nullptr);

	if (pData == nullptr || pNewData == nullptr)
	{
		return false;
	}

	int iMinLen = 0; // 包头的大小

	char *cbData = (char *)pData;
	char cbMask[4] = { 0 };
	

	// 长度小于包头
	if (size < getPacketHeadSize())
	{
		return false;
	}
    
    // websocket 关闭
    int opCode = cbData[0] & 0xf;
    std::cout << "opCode: " << opCode << std::endl;
    if (opCode == 8) {
        return false;
    }

	int len = cbData[1] & ((1 << 7) ^ 0xff);

	bool bMask = (cbData[1] & (1 << 7)) >> 7;
    std::cout << "bMask: " << bMask << std::endl;

	// 长度的偏移
	int offset = (len == 126) ? 2 : (len == 127) ? 8 : 0;
	offset += getPacketHeadSize();

	iMinLen += offset;

	if (bMask) {
		iMinLen += 4;
	}

	assert(size >= iMinLen);
	if (size < iMinLen)
	{
		return false;
	}

	// 提取key
	if (bMask)
	{
		memcpy(cbMask, cbData + offset, 4);
	}

	// 解析数据
	int _recvDataSize = size - iMinLen;
	char *_pNewData = (char *)pNewData;

	for (size_t i = 0; i < _recvDataSize; i++)
	{
		int _offset = i + iMinLen;

		assert(*pNewDataSize >= i + 1);

		if (*pNewDataSize < i + 1) 
		{
			// 缓冲区大小不够
			return false;
		}

		if (bMask)
		{
			_pNewData[i] = cbData[_offset] ^ cbMask[i % 4];
		}
		else
		{
			_pNewData[i] = cbData[_offset];
		}
	}


	*pNewDataSize = _recvDataSize;

	return true;
}

// 加密数据
bool WebSocketProtocol::encodeData(const void *pData, int wDataSize, void *pNewData, int *pNewDataSize)
{
	std::cout << "WebSocketProtocol::encodeData" << std::endl;
	assert(pData != nullptr);
	assert(pNewData != nullptr);
	assert(wDataSize >= 0);
	
	if (pData == nullptr || pNewData == nullptr || wDataSize < 0)
	{
		return false;
	}

	// 最多12个扩展长度
	const int extDataLen = 12;
	const int32_t xorKey = 0x80ccddb3;
	/*
		| F| R | R| R | opcode | M | Payload len | Extended payload length    |
		| I | S | S | S |           (4) | A | 		(7)			  | (16 / 64)                                       |
		|N| V| V| V |                  | S |                           | (if payload len == 126 / 127)|
		|   | 1 | 2 | 3 |                  | K |                           |                                                         |
		*/
	const unsigned char fin = (1 << 7) + 2; // (fin+rsv1+rsv2+rsv3+opcode) 2 代表二进制  1 代表 字符串

	char extData[extDataLen] = { 0 };

	WebsocketHead _ws_head;
	memset(&_ws_head, 0, sizeof(WebsocketHead));
	_ws_head.fin = fin;

	int _extLen = 0;


	//char mask = 1 << 7;
	char mask = 0; // 没有掩码
	if (wDataSize < 126)
	{
		mask += wDataSize;
		//_extLen = sizeof(int32_t);
	}
	else if (wDataSize >= 126 && wDataSize <= 0xFFFF) {
		mask += 126;
		_extLen = sizeof(WebsocketExt);
		((WebsocketExt *)&extData)->len = htons(wDataSize);
		//((WebsocketExt *)&extData)->mask = xorKey;
	}
	else {
		mask += 127;
		_extLen = sizeof(WebsocketExt2);
		((WebsocketExt2 *)&extData)->len = HTONLL(wDataSize);
		//((WebsocketExt2 *)&extData)->mask = xorKey;
	}

	_ws_head.mask = mask;

	int headLen = sizeof(WebsocketHead) + _extLen;
	int packetlen = headLen + wDataSize;

	assert(*pNewDataSize >= packetlen);

	if (*pNewDataSize < packetlen) // 缓冲区大小不够
	{
		std::cout << "缓冲区大小不够" << std::endl;
		
		return false;
	}

	std::cout << "缓冲区大小：" << *pNewDataSize << std::endl;
	std::cout << "包大小: " << packetlen << std::endl;


	char *_pNewData = (char *)pNewData;
	//char *pXorKey = (char *)&xorKey;

	memcpy(_pNewData, &_ws_head, sizeof(WebsocketHead)); // 写入头

	//if (_extLen == sizeof(int32_t)) {
	//	memcpy(_pNewData + sizeof(WebsocketHead), &xorKey, _extLen); // 写入长度
	//}
	//else {
	//	memcpy(_pNewData + sizeof(WebsocketHead), &extData, _extLen); // 写入长度
	//}
	//

	if (_extLen != 0) // 写入长度扩展
	{
		memcpy(_pNewData + sizeof(WebsocketHead), &extData, _extLen);
	}

	// 写入包内容
	memcpy(_pNewData + headLen, pData, wDataSize);
	
	//for (size_t i = 0; i < wDataSize; i++)
	//{
	//	_pNewData[i + headLen] = pdata[i] ^ pXorKey[i % 4];
	//}

	*pNewDataSize = packetlen;

	std::cout << "WebSocketProtocol::encodeData success" << std::endl;
	return true;
}



void TCPNetworkDataCache::resetData()
{
	std::cout << "TCPNetworkDataCache::resetData" << std::endl;
    if (_dataBuffer != nullptr)
    {
        POLL_FREE(_dataBuffer);
    }
    _dataBuffer = nullptr;
    _dataBufferSize = 0;
    _dataBufferIndex = 0;
    _dataBufferUseSize = 0;
}


// 添加缓冲数据
bool TCPNetworkDataCache::addDataBuffer(const void *pdata, size_t size)
{
	assert(pdata);
	assert(size);

	if (!pdata || !size) 
	{
		std::cout << "TCPNetworkDataCache::addDataBuffer param error" << std::endl;

		return false;
	}
	std::cout << "TCPNetworkDataCache::addDataBuffer" << std::endl;
    if (_dataBuffer == nullptr) { // 没有数据
        size_t count = ((size + kChunkSize - 1) / kChunkSize) * kChunkSize;
        char *pDataBuffer = (char *)POLL_ALLOC(count);

		std::cout << "_dataBuffer == nullptr" << std::endl;
		std::cout << "POLL_ALLOC size = " << count << std::endl;
		std::cout << (void *)pDataBuffer << std::endl;
        
        if (pDataBuffer == NULL) {
			std::cout << "POLL_ALLOC alloc failure" << std::endl;
            return false;
        }
        _dataBuffer = pDataBuffer;
		_dataBufferSize = count;
    }
    
    // 内存大小够用
    if (_dataBufferIndex + _dataBufferUseSize + size <= _dataBufferSize) {
		std::cout << "addDataBuffer 内存大小够用" << std::endl;
        
        memcpy(_dataBuffer + _dataBufferIndex + _dataBufferUseSize, pdata, size);
        
        _dataBufferUseSize += size;
        return true;
        
    }
    else if (_dataBufferUseSize + size > _dataBufferSize) { // 内存大小不够使用
		std::cout << "addDataBuffer 内存大小不够用" << std::endl;
        size_t _size = size + _dataBufferUseSize;
        
        size_t count = ((_size + kChunkSize - 1) / kChunkSize) * kChunkSize;
        char *pDataBuffer = (char *)POLL_ALLOC(count);
        
        if (pDataBuffer == NULL) {
            return false;
        }
        
        memcpy(pDataBuffer, _dataBuffer + _dataBufferIndex, _dataBufferUseSize); // 拷贝内存
        POLL_FREE(_dataBuffer);
        _dataBuffer = pDataBuffer;
		_dataBufferSize = count;
        _dataBufferIndex = 0;
        return addDataBuffer(pdata, size); // 重新添加数据
    }
    else if (_dataBufferUseSize + size <= _dataBufferSize
             && _dataBufferIndex + _dataBufferUseSize + size > _dataBufferSize){
		std::cout << "addDataBuffer 内存数据需要整理" << std::endl;
        memmove(_dataBuffer, _dataBuffer + _dataBufferIndex, _dataBufferUseSize);
        _dataBufferIndex = 0;
        return addDataBuffer(pdata, size); // 重新添加数据
    }
    
    return false;
}


bool TCPNetworkDataCache::freeDataBuffer()
{
	std::cout << "TCPNetworkDataCache::freeDataBuffer" << std::endl;
   // POLL_FREE(_dataBuffer);
    resetData();
    
    return true;
}





