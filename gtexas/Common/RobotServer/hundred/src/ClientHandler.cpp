#include "ClientHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "GameCmd.h"
#include "GameServerConnect.h"

using namespace std;


ClientHandler::ClientHandler()
{
	callback_count = 0;
	for(int i=0; i<MAX_CALL_BACK; ++i)
			callback_seq[i] = 0;
}

ClientHandler::~ClientHandler() 
{
	//this->_decode = NULL;
	LOGGER(E_LOG_INFO) << "client handler address = " << this;
}

int ClientHandler::OnClose()
{
	LOGGER(E_LOG_INFO) << "client ip = " << m_addrremote << " port = " << m_nPort << " fd = " << GetNetfd();
	return 0;
}

int ClientHandler::OnPacketComplete(const char* data, int len)
{
	pPacket.Copy(data,len);
	return Despatch::getInstance()->doFrontRequest(this,&pPacket);

}

int ClientHandler::OnConnected(void)
{
	GetRemoteAddr();//得到远程连接的地址和端口
	LOGGER(E_LOG_INFO) << "client ip = " << m_addrremote << " port = " << m_nPort << " fd = " << GetNetfd();
	return 0;
}
void ClientHandler::GetRemoteAddr(void)
{
	struct in_addr sin_addr;
	sin_addr.s_addr = this->_ip;
	char * addr = inet_ntoa(sin_addr);
	m_nPort = this->_port;
	
	if(addr)
		m_addrremote = string(addr);
	else
		m_addrremote = "NULL";
}


int ClientHandler::addCallbackSeq(unsigned short seq)
{
	if(callback_count==MAX_CALL_BACK)
	{
		LOGGER(E_LOG_ERROR) << "[CALL_BACK] too much callback wait for prcess. callback count = " << callback_count << " >= " << MAX_CALL_BACK;
		return -1;
	}
	for(int i=0; i<MAX_CALL_BACK; ++i)
	{
		if(callback_seq[i] == 0)
		{
			callback_seq[i] = seq;
			callback_count++;
			LOGGER(E_LOG_DEBUG) << "addCallbackSeq seq=[" << seq << " count = " << callback_count;
			return 0;
		}
	}
	LOGGER(E_LOG_ERROR) << "[CALL_BACK] too much callback wait for prcess. callback count = " << callback_count << " >= " << MAX_CALL_BACK;
	return -1;
}

void ClientHandler::deleteCallbackSeq(unsigned short seq)
{
	for(int i=0; i<MAX_CALL_BACK; ++i)
	{
		if(callback_seq[i] == seq)
		{
			callback_seq[i] = 0;
			callback_count--;
			LOGGER(E_LOG_DEBUG) << "deleteCallbackSeq seq=[" << seq << " count = " << callback_count;
			return;
		}
	}
}

void ClientHandler::clearCallbackSeq()
{
	LOGGER(E_LOG_DEBUG) << "clearCallbackSeq  count=[" << callback_count;
	
	for(int i=0; i<MAX_CALL_BACK; ++i)
	{
		if(callback_seq[i] != 0)
		{
			unsigned short seq = callback_seq[i] ;  
			Despatch* game_server = Despatch::getInstance();
			game_server->deleteCallBackParm(seq);
			callback_seq[i] = 0;
			callback_count--;
		}
	}

}
