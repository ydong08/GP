#include "Despatch.h"
#include "BaseProcess.h"
#include "Configure.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "Player.h"
#include "PlayerManager.h"
#include "HallHandler.h"

#define MAX_CONTEXT 65536

using namespace std;

Despatch::Despatch()
{
	//seq = 0;
	//context = (Context*)malloc(MAX_CONTEXT, sizeof(Context));
}

Despatch::~Despatch()
{

}

// static void initContext(Context* pt , unsigned int seq, IProcess* processer, CDLSocketHandler* clientHandler, InputPacket* pPacket)
// {
// 	pt->seq = seq;
// 	pt->status = 0;
// 	pt->process =processer;
// 	pt->client = clientHandler;
// 	if(pPacket)
// 		memcpy(&pt->msgHeader, pPacket->packet_buf(), sizeof(PacketHeader));
// 	else
// 		memset(&pt->msgHeader, 0, sizeof(PacketHeader));
// 	if (pt->data)
// 	{
// 		free(pt->data);
// 		pt->data = NULL;
// 	}
// }
 
int Despatch::doFrontRequest(CDLSocketHandler* clientHandler,  InputPacket* pPacket)
{
	if (clientHandler == NULL || pPacket == NULL)
	{
		LOGGER(E_LOG_WARNING) << "clientHandler is null or pPacket is null";
		return -1;
	}
	short nCmdType = pPacket->GetCmdType();
	int   uid = pPacket->GetUid();
	ULOGGER(E_LOG_DEBUG, uid) << "Request: fd=" << clientHandler->GetNetfd() << " Cmd=" << nCmdType << " GameId=" << pPacket->GetOptType();
	if (pPacket->CrevasseBuffer() == -1)
	{
		ULOGGER(E_LOG_WARNING, uid) << "DATA CrevasseBuffer ERROR data decrypt.";
		return 0;
	}

	BaseProcess* processor = ProcessManager::getInstance().getProcesser(nCmdType);	 
	if (processor)
	{
//		unsigned short seqNo = ++this->seq;
// 		if(seqNo==0)
// 			seqNo = ++this->seq;

// 		Context* pt = &context[seqNo];
// 		initContext( pt, seq, processer, clientHandler, pPacket );
		int ret = processor->doRequest( clientHandler, pPacket, NULL);
		if (ret<0)
		{
			ULOGGER(E_LOG_WARNING, uid) << "Process Error ret=" << ret;
		}
	}
	else
	{
		ULOGGER(E_LOG_ERROR, uid) << "[doFrontRequest] Can't find Process ,cmd="<< nCmdType;
	}
	return 0;
}

int Despatch::doBackResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket)
{
	if (clientHandler == NULL || pPacket == NULL)
	{
		LOGGER(E_LOG_WARNING) << "clientHandler is null or pPacket is null";
		return -1;
	}
	int uid = pPacket->GetUid();
	short nCmdType = pPacket->GetCmdType();
	unsigned short sn = pPacket->GetSeqNum();
	ULOGGER(E_LOG_DEBUG, uid) << "Response: fd=" << clientHandler->GetNetfd() << " GameID=" << pPacket->GetOptType() << " Cmd=" << nCmdType << " sn=" << sn;
	if (pPacket->CrevasseBuffer() == -1)
	{
		ULOGGER(E_LOG_WARNING, uid) << "DATA CrevasseBuffer ERROR data decrypt.";
		return -1;
	}

	BaseProcess* processer = GET_PROCESS(nCmdType);
	int ret = 0;
	if (processer) {
		ret = processer->doResponse(clientHandler, pPacket, NULL);
	} else { 
		ULOGGER(E_LOG_ERROR, uid) << "Can't find Process ,cmd=" << nCmdType;
	}
	
	if (ret == 0)
	{
		return 0;
	}

	if (ret == EXIT)
	{
		HallHandler* hallhandler = dynamic_cast<HallHandler*>(clientHandler);
		Player* player = PlayerManager::getInstance().getPlayer(hallhandler->uid);
		if (player == NULL) {
			ULOGGER(E_LOG_ERROR, uid) << "not found player id=" << hallhandler->uid;
			return 0;
		}
		player->logout();
		PlayerManager::getInstance().delPlayer(player->id, true);
		return 0;
	}
	
	processer = GET_PROCESS(ret);
	if (processer)
	{
	  processer->doRequest(clientHandler, NULL, NULL);
	}
	else
	{
		ULOGGER(E_LOG_ERROR, uid) << "[doFrontRequest] Can't find Process ,cmd=" << nCmdType;
	}
	return 0;
}

// void Despatch::deleteCallBackParm(int seq, int uid)
// {
// 	_LOG_DEBUG_("deleteCallBackParm: initContext[%d] uid[%d]\n", seq, uid);
// 	Context* pt = &context[seq];
// 	if (uid != 0)
// 	{
// 		int ptUid = htonl(pt->msgHeader.uid);
// 		if (ptUid == uid)
// 			initContext(pt, seq, NULL, NULL, NULL);
// 		else
// 			_LOG_WARN_("deleteCallBackParm: initContext[%d] uid[%d] Not equal ptUID[%d]\n", seq, uid, ptUid);
// 	}
// 	else
// 		initContext(pt, seq, NULL, NULL, NULL);
// }
