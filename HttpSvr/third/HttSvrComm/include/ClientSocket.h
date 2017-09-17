
#ifndef __CLIETN_SOCKET_H__
#define __CLIETN_SOCKET_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

class CClientSocket
{
public:
	CClientSocket();
	CClientSocket(const std::string& host,const int port, const unsigned int nTimeOutMicroSend = 300000, const unsigned int nTimeOutMicroRecv = 300000);
	virtual ~CClientSocket();

	bool Connect(const std::string& host,const int port, const unsigned int nTimeOutMicroSend = 300000, const unsigned int nTimeOutMicroRecv = 300000);
	bool ReConnect();
	int Send(char* szBuff, int nLen);
	int Recv(char*& szBuffRet);
	int Recv(std::string& message);
	bool IsValid() const;
private:
	void socket_close(int nFd);
	int socket_create(const int nAf, const int nType, const int nProtocol, const unsigned int nTimeOutMicroSend, const unsigned int nTimeOutMicroRecv);
	
	int m_sockfd;
	std::string m_strSvrIp;
	int m_nSvrport;

	unsigned int m_nTimeOutMicroSend;
	unsigned int m_nTimeOutMicroRecv;
};

#endif