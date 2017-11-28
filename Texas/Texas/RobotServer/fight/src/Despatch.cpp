#include "Despatch.h"
#include "IProcess.h"
#include "Configure.h"
#include "Logger.h"
#include "ProcessFactory.h"
#include "GameCmd.h"
#include "Player.h"
#include "PlayerManager.h"
#include "HallHandler.h"

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

static void initContext(Context* pt , unsigned int seq, IProcess* processer, CDLSocketHandler* clientHandler, InputPacket* pPacket)
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
	Configure::instance()->lasttime = time(NULL);
	short nCmdType = pPacket->GetCmdType();
	LOGGER(E_LOG_DEBUG) << "Recv Front Request, Packet From Source" << pPacket->GetSource() << " cmd = " << nCmdType;
	//_LOG_DEBUG_("==============WorkProcess[%d]:doFrontRequest =============\n",getpid());
	//_LOG_DEBUG_( "Recv Front Request, Packet From Source[%d] Cmd[0x%04x]\n" ,pPacket->GetSource(),nCmdType) ;
	if (pPacket->CrevasseBuffer() == -1)
	{
		LOGGER(E_LOG_DEBUG) << "[DATA CrevasseBuffer ERROR] data decrypt error";
		return -1;
	}

	IProcess* processer = ProcessFactory::getProcesser(nCmdType);
	 
	if(processer)
	{
		unsigned short seqNo = ++this->seq;
		if(seqNo==0)
			seqNo = ++this->seq;

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
	Configure::instance()->lasttime = time(NULL);
	short nCmdType = pPacket->GetCmdType();
	unsigned short sn = pPacket->GetSeqNum();
	LOGGER(E_LOG_DEBUG) << "Recv Back Response, Packet From Cmd = " << nCmdType << " Source = " << pPacket->GetSource() << " sn = " << sn;
	//_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse =============\n",getpid());
	//_LOG_DEBUG_( "Recv Back Response, Packet From Cmd[0x%04x], Source[%d], SN[%d]\n" ,nCmdType,pPacket->GetSource(), sn) ;
	if (pPacket->CrevasseBuffer() == -1)
	{
		LOGGER(E_LOG_ERROR) << "data decrypt error";
		return -1;
	}

	IProcess* processer = ProcessFactory::getProcesser(nCmdType);
	int ret = 0;
	if(processer)
		ret = processer->doResponse( clientHandler, pPacket, NULL);
	else
		//_LOG_INFO_( "[doBackResponse] Can't find Process ,cmd=[0x%04x]\n" ,nCmdType) ;
		LOGGER(E_LOG_ERROR) << "Can't find Process ,cmd = " << nCmdType;

#ifndef WIN32
	LOGGER(E_LOG_DEBUG) << "==============WorkProcess" << getpid() << ":doBackResponse OK===========";
#endif

	if(ret == EXIT)
	{
		HallHandler* hallhandler = reinterpret_cast <HallHandler*> (clientHandler);
		Player* player = PlayerManager::getInstance()->getPlayer(hallhandler->uid);
		if(player == NULL)
			return -1;
		player->logout();
		PlayerManager::getInstance()->delPlayer(player->id);
		return -1;
	}

	processer = ProcessFactory::getProcesser(ret);

	if(processer)
	{
	  //_LOG_DEBUG_("================doRequest================\n");
	  //_LOG_DEBUG_("Next=[0x%04x]\n", ret);
	  LOGGER(E_LOG_DEBUG) << "================doRequest================ next = " << ret;
	  processer->doRequest(clientHandler, NULL, NULL);
	}
	else
	{
		//_LOG_DEBUG_("Next=[0x%04x]\n", ret);
		//_LOG_DEBUG_("No Next Process, Wait For Server Response\n");
		LOGGER(E_LOG_DEBUG) << "next = " << ret << "No Next Process, Wait For Server Response";

	}
	//_LOG_DEBUG_("=======================================\n\n");
	 
	//_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse OK===========\n\n",getpid());
#ifndef WIN32
	LOGGER(E_LOG_DEBUG) << "==============WorkProcess" << getpid() << ":doBackResponse OK===========";
#endif

	return 0;
}


void Despatch::deleteCallBackParm(int seq, int uid)
{
	_LOG_DEBUG_("deleteCallBackParm: initContext[%d] uid[%d]\n",seq,uid);
	Context* pt = &context[seq];
	if(uid != 0)
	{
		int ptUid = htonl(pt->msgHeader.uid);
		if(ptUid == uid)
			initContext( pt, seq, NULL, NULL, NULL );
		else
			ULOGGER(E_LOG_WARNING, uid) << "initContext = " << seq << " Not equal ptUID = " << ptUid;
	}
	else
		initContext( pt, seq, NULL, NULL, NULL );
}
