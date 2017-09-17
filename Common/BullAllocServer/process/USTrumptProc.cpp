#include "USTrumptProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "AllocManager.h"
#include "BackConnect.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "TrumptConnect.h"

USTrumptProc::USTrumptProc()
{
	this->name = "USTrumptProc";
}

USTrumptProc::~USTrumptProc()
{

} 
int USTrumptProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();

	_LOG_INFO_("==>[USTrumptProc] [0x%04x] uid[%d]\n", cmd, uid);

	//把当前包的序列号设置为当前进程的序列号
	pPacket->SetSeqNum(pt->seq);
	//这里要把当前这个包的seq放到回调里面，方便继续给前端发送包
	pt->seq = seq;

	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);

	if(TrumptConnect::getInstance()->Send(pPacket)>=0)
	{
		_LOG_DEBUG_("Transfer USTrumptProc request to Back_UserServer OK\n" );
		return 1;
	}
	else
	{
		sendErrorMsg(clientHandler, cmd, uid, -22,ErrorMsg::getInstance()->getErrMsg(-22),seq);
		return 0;
	}

	
	return 0;
}

int USTrumptProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	if(pt==NULL)
	{
       _LOG_ERROR_("[USTrumptProc:doResponse]Context is NULL\n");	
	   return -1;
	}
	
	OutputPacket response ;
	response.Copy(inputPacket->packet_buf(),inputPacket->packet_size());
	response.SetSeqNum(pt->seq);//把此包的序列号重新置回先前传入的seq
	response.End();	
	if(pt->client)
	{
		int n = this->send(pt->client, &response, false);
		_LOG_DEBUG_("Send to Hall  success, len=[%d]\n", n);
	}
	else
		_LOG_DEBUG_("pt->client == NULL\n" );

	return 0;
}
