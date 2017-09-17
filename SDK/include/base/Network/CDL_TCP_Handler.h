#ifndef  _CDL_TCP_Handler_H_
#define  _CDL_TCP_Handler_H_

#include "poller.h"

class CDL_TCP_Handler : public CPollerObject 
{
	public:
		CDL_TCP_Handler(){};
		virtual ~CDL_TCP_Handler(){};
 
	public:
		virtual int OnClose()=0;				// 连接断开后调用 注意:如果返回1则不会删除对象只会关闭连接
		virtual int OnConnected()=0;		// 连接成功建立后调用
		virtual int OnPacketComplete(const char * data, int len)=0; // 需解析完整数据包时调用
		
	public:
		inline int GetIP(){return this->_ip;}; 
		inline void SetIP(int ip){ this->_ip = ip;}; 

		inline uint16_t GetPort(){return this->_port;}; 
		inline void SetPort(uint16_t port){ this->_port = port;}; 

		inline int GetNetfd(){return this->netfd;}; 
		inline void SetNetfd(int netfd){ this->netfd = netfd;}; 

	protected:
		in_addr_t	_ip;
		uint16_t	_port;

};
#endif

