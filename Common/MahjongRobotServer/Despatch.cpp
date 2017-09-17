#include "Despatch.h"
#include "Configure.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "Player.h"
#include "PlayerManager.h"
#include "HallHandler.h"
#include "Packet.h"
#include "BaseProcess.h"
#include "ClientHandler.h"

#define MAX_CONTEXT 65536

using namespace std;

static Despatch* instance = NULL;


Despatch* Despatch::getInstance()
{
	if(instance==NULL)
	{
		instance = new Despatch();
	}
	return instance;
}

Despatch::Despatch()
{
	seq = 0;
	context = (Context*) calloc(MAX_CONTEXT,sizeof(Context));
}

static void initContext(Context* pt , unsigned int seq, BaseProcess* processer, CDLSocketHandler* clientHandler, InputPacket* pPacket)
{
	pt->seq = seq;
	pt->status = 0;
	pt->process =processer;
	pt->client = clientHandler;
	if(pPacket)
		memcpy(&pt->msgHeader,pPacket->packet_buf(), sizeof(PacketHeader));
	else
		memset(&pt->msgHeader,0,sizeof(PacketHeader));
	if(pt->data)
	{
		free(pt->data);
		pt->data = NULL;
	}
}
 
int Despatch::doFrontRequest(CDLSocketHandler* clientHandler,  InputPacket* pPacket)
{
	short nCmdType = pPacket->GetCmdType();
	//LOGGER(E_LOG_DEBUG) << "Recv Front Request, Packet From Source" << pPacket->GetSource() << " cmd = " << nCmdType;
	if(nCmdType != START_ROBOT)
	{
		if (pPacket->CrevasseBuffer() == -1)
		{
			LOGGER(E_LOG_DEBUG) << "[DATA CrevasseBuffer ERROR] data decrypt error" << " cmd = " << nCmdType;
			return -1;
		}
	}

	BaseProcess* processer = ProcessManager::getInstance().getProcesser(nCmdType);
	 
	if(processer)
	{
		unsigned short seqNo = ++this->seq;
		if(seqNo==0)
			seqNo = ++this->seq;

		//_LOG_DEBUG_("+++++++++++processer[%s]++++++++++++++++++\n", processer->name);
		Context* pt = &context[seqNo];
		initContext( pt, seq, processer, clientHandler, pPacket );

		int ret = processer->doRequest( clientHandler, pPacket, pt);
		
		if(ret<0)
		{
			LOGGER(E_LOG_ERROR) << "Process Error ret = " << ret;
		}
		else if(ret>0)
		{
			//clientHandler中添加回调任务序号，以备异常退出时清空回调任务
			 ((ClientHandler*)clientHandler)->addCallbackSeq(seqNo);
			LOGGER(E_LOG_DEBUG) << "Send to BackServer,Wait for  BackServer Response. SN = " << seqNo;
		}
	}
	else
	{
		LOGGER(E_LOG_ERROR) << "Can't find Process ,cmd = " << nCmdType;
	}
#ifndef WIN32
	LOGGER(E_LOG_DEBUG) << "==============WorkProcess" << getpid() << ":doFrontRequest OK===========";
#endif
	 
	return 0;
}

int Despatch::doBackResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket)
{
	short nCmdType = pPacket->GetCmdType();
	unsigned short sn = pPacket->GetSeqNum();
	if (113 != nCmdType)
		LOGGER(E_LOG_DEBUG) << "Recv Back Response, Packet From Cmd = " << nCmdType << " Source = " << pPacket->GetSource() << " sn = " << sn;
	if (pPacket->CrevasseBuffer() == -1)
	{
		LOGGER(E_LOG_ERROR) << "data decrypt error";
		return -1;
	}

	BaseProcess* processer = ProcessManager::getInstance().getProcesser(nCmdType);
	if (NULL == processer)
		return 0;

	int ret = processer->doResponse(clientHandler, pPacket, NULL);
	if(ret == EXIT)
	{
		HallHandler* hallhandler = reinterpret_cast <HallHandler*> (clientHandler);
		Player* player = PlayerManager::getInstance()->getPlayer(hallhandler->uid);
		if(player == NULL)
			return 0;

		ULOGGER(E_LOG_INFO, player->m_Uid) << "stop robot, tid = " << player->m_TableId;
		player->logout();
		PlayerManager::getInstance()->delPlayer(player->m_Uid);
		return 0;
	}

	processer = ProcessManager::getInstance().getProcesser(ret);
	if(processer)
		processer->doRequest(clientHandler, NULL, NULL);
	
	return 0;
}

void Despatch::deleteCallBackParm(int seq, int uid)
{
	ULOGGER(E_LOG_DEBUG, uid) << "initContext = " << seq;
	Context* pt = &context[seq];
	if(uid != 0)
	{
		int ptUid = htonl(pt->msgHeader.uid);
		if (ptUid == uid)
		{
			initContext(pt, seq, NULL, NULL, NULL);
		}
		else
		{
			ULOGGER(E_LOG_WARNING, uid) << "initContext = " << seq << " Not equal ptUID = " << ptUid;
		}
	}
	else
	{
		initContext(pt, seq, NULL, NULL, NULL);
	}
}
