
#ifndef __CLIETN_SOCKET_EX_H__
#define __CLIETN_SOCKET_EX_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "Packet.h"
#include "ClientSocket.h"

class CClientSocketEx : public CClientSocket
{
public:
	CClientSocketEx();
	CClientSocketEx(const std::string& host,const int port, const unsigned int nTimeOutMicroSend = 300000, const unsigned int nTimeOutMicroRecv = 300000);
	~CClientSocketEx();


	int SendEx(OutputPacket* outPack, bool isEncrypt=true, bool bReConnect = true);
	int RecvEx(InputPacket& pPackRecv, bool bReConnect = true);
};

#endif