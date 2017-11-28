#include "ClientSocketEx.h"
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <fcntl.h>
//#include <stdio.h>
#include <unistd.h>

#include "Logger.h"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

//每次读数据的最大长度, 注意：这里不是限制包的总长度，纯粹是为了分段读数据
#define MaxOnceRecv 1024
#define MaxOnceSend 1024

CClientSocketEx::CClientSocketEx() : CClientSocket()
{
}
CClientSocketEx::CClientSocketEx(const std::string& host,const int port, const unsigned int nTimeOutMicroSend, const unsigned int nTimeOutMicroRecv) : CClientSocket(host,port,nTimeOutMicroSend,nTimeOutMicroRecv)
{
}


CClientSocketEx::~CClientSocketEx()
{
}
int CClientSocketEx::SendEx( OutputPacket* outPack, bool isEncrypt/*=true*/, bool bReConnect /*= true*/ )
{
	if(!IsValid()) ReConnect();

	if(isEncrypt) outPack->EncryptBuffer();
	int nSend = Send((char*)outPack->packet_buf(),outPack->packet_size());
	if (nSend != outPack->packet_size() && bReConnect)
	{
		//重连，然后再发送一次
		if(!ReConnect()) return -1;
		nSend = Send(outPack->packet_buf(),outPack->packet_size());
	}
	return nSend;
}
int CClientSocketEx::RecvEx( InputPacket& pPackRecv, bool bReConnect /*= true*/ )
{
	if(!IsValid()) 
	{
		if(bReConnect) ReConnect();
		return -1;			//重连后直接退出，因为服务器不会向新的scoket发消息的
	}

	char* szBuffRet = NULL;
	int nRead = Recv(szBuffRet);
	if (NULL == szBuffRet)
	{
		if(bReConnect) ReConnect();
		return -1;		//重连后直接退出，因为服务器不会向新的scoket发消息的
	}

	pPackRecv.Copy(szBuffRet,nRead);
	if (szBuffRet) delete[] szBuffRet;

	return nRead;
}