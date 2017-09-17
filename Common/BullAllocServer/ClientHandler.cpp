#include "ClientHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "GameCmd.h"
#include "HallManager.h"
#include "ServerManager.h"
#include "AllocManager.h"
#include "BackConnect.h"

using namespace std;

ClientHandler::ClientHandler()
{
	id =-1;
	type = NONSRV_HANDLE ;
	keepalivetime = time(NULL);
	callback_count = 0;
	for(int i=0; i<MAX_CALL_BACK; ++i)
			callback_seq[i] = 0;
}

ClientHandler::~ClientHandler() 
{
	//this->_decode = NULL;
	//socket关闭，析构时清除为完成的回调任务，避免野指针异常
	clearCallbackSeq();
}

int ClientHandler::OnClose()
{
	_LOG_DEBUG_("[onclose] client[%s:%d] fd[%d], id[%d]\n", m_addrremote.c_str(), m_nPort, GetNetfd(), id);
	if(type==HALLSRV_HANDLE && id!=-1)
	{
		HallManager::getInstance()->delHallHandler(id);
	}
	else if(type==GMSRV_HANDLE && id!=-1)
	{
		logOutAllUsers();
		ServerManager::getInstance()->delServer(id);
		ServerManager::getInstance()->delGameServerHandler(id);
		AllocManager::getInstance()->delTablesInSvid((short)id);
	}
	id = -1;
	type = NONSRV_HANDLE;
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
	_LOG_DEBUG_("[onconnect] client[%s:%d] fd[%d]\n", m_addrremote.c_str(), m_nPort, GetNetfd() );
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

void ClientHandler::logOutAllUsers()
{
	// 为gameserver时可用
	if(type==GMSRV_HANDLE && id!=-1)
	{
		GameServer * gameServer = ServerManager::getInstance()->getServer(id);

		OutputPacket outPacket;
		outPacket.Begin(ALLOC_LOGOUT_ALL_USERS);
		outPacket.WriteInt(gameServer->svid);
		outPacket.WriteInt(gameServer->gameID);
		outPacket.WriteInt(gameServer->level);
		outPacket.End();

		std::map<int, BackNodes*> * backNodess = BackConnectManager::getNodess();
		if (backNodess==0)
		{
			return;
		}

		std::map<int, BackNodes*>::iterator it;
		for (it = backNodess->begin();it != backNodess->end();++it)
		{
			BackNodes* backNodes = it->second;
			int nRet = backNodes->send(&outPacket);
			if(nRet >0 )
			{
				_LOG_DEBUG_("logOutAllUsers OK,backNodes[%d]\n",it->first);
			}
			else
			{
				_LOG_ERROR_("logOutAllUsers Error,backNodes[%d]\n",it->first );
			}
		}

	}
}


int ClientHandler::addCallbackSeq(unsigned short seq)
{
	if(callback_count==MAX_CALL_BACK)
	{
		_LOG_ERROR_("[CALL_BACK] too much callback wait for prcess. callback count [%d] >= [%d]!!\n", callback_count,MAX_CALL_BACK );
		return -1;
	}
	for(int i=0; i<MAX_CALL_BACK; ++i)
	{
		if(callback_seq[i] == 0)
		{
			callback_seq[i] = seq;
			callback_count++;
			_LOG_DEBUG_("addCallbackSeq seq=[%d] count=[%d] \n", seq, callback_count);
			return 0;
		}
	}
	_LOG_ERROR_("[CALL_BACK] too much callback wait for prcess. callback count [%d] >= [%d]!!\n", callback_count,MAX_CALL_BACK );
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
			_LOG_DEBUG_("deleteCallbackSeq seq=[%d] count=[%d] \n", seq, callback_count);
			return;
		}
	}
}

void ClientHandler::clearCallbackSeq()
{
	_LOG_DEBUG_("clearCallbackSeq  count=[%d] \n",  callback_count);

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
