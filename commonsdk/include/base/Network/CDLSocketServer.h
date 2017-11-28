#ifndef  _CDLSocketServer_H_
#define _CDLSocketServer_H_

#include "CDL_TCP_Server.h"
#include "CDL_TCP_Handler.h"

 
class CDLSocketServer  : public CDL_TCP_Server 
{
	public:
		CDLSocketServer(const char* bindIp, uint16_t port, int acceptcnt, int backlog);
		virtual ~CDLSocketServer();
 
		virtual int Init();
		virtual int InputNotify();
		
		protected:
			int	_accept_cnt;
			int	_newfd_cnt;
			int*	_fd_array;
			struct sockaddr_in*  _peer_array;

			uint16_t	_bindPort;
			char	_bindAddr[128];
			int	_backlog;
			unsigned int	_flag;

			virtual CDL_TCP_Handler* CreateHandler(int netfd, struct sockaddr_in* peer)=0;

			int proc_accept (struct sockaddr_in* peer, socklen_t* peerSize);
			int proc_request (struct sockaddr_in* peer);
};

template <class T>
class SocketServer  : public CDLSocketServer 
{
	public:
		SocketServer(const char* bindIp, uint16_t port, int acceptcnt=256, int backlog=256)
			:CDLSocketServer(bindIp,   port,   acceptcnt,   backlog)
		{};
		virtual ~SocketServer(){};

	protected:
		virtual CDL_TCP_Handler* CreateHandler(int netfd, struct sockaddr_in* peer)
		{
			return new T;
		}
};

#endif

