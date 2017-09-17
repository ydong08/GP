#include "ChangeTabProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "AllocManager.h"
#include "BackConnect.h"
#include "Configure.h"
#include "ErrorMsg.h"
//#include "UdpManager.h"

ChangeTabProc::ChangeTabProc()
{
	this->name = "ChangeTabProc";
}

ChangeTabProc::~ChangeTabProc()
{

} 

int ChangeTabProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short level = pPacket->ReadShort();
	int64_t money = pPacket->ReadInt64();

	_LOG_INFO_("==>[ChangeTabProc] [0x%04x] uid[%d] tid[%d] \n", cmd, uid, tid);
	_LOG_DEBUG_("[DATA Parse] level=[%d] \n", level);

	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	if(level != Configure::instance()->m_nLevel)
	{
		_LOG_ERROR_("Level[%d] is not equal AllocLevel[%d]\n", level, Configure::instance()->m_nLevel);
		return sendErrorMsg(clientHandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
	}
	//把当前包的序列号设置为当前进程的序列号
	pPacket->SetSeqNum(pt->seq);
	//这里要把当前这个包的seq放到回调里面，方便继续给前端发送包
	pt->seq = seq;
	
	//到用户服务器去检测，主要解决到此用户强退的时候再匹配也要到原来的那个房间里面去
	if(BackConnectManager::getNodes(uid%10 + 1)->send(pPacket)>0)
	{
		_LOG_DEBUG_("Transfer[%d] ChangeTabProc request to Back_UserServer OK\n" ,uid);
		return 1;
	}
	else
	{
		_LOG_ERROR_("Transfer[%d] ChangeTabProc request to Back_UserServer Error\n",uid );
		return -1;
	}
	
	return 0;
}

int ChangeTabProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	if(pt==NULL)
	{
       _LOG_ERROR_("[ChangeTabProc:doResponse]Context is NULL\n");	
	   return -1;
	}

	short cmd = inputPacket->GetCmdType();
	short retno = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	_LOG_INFO_("==>[ChangeTabProc] doResponse [0x%04x] retno=%d\n", cmd, retno);
	_LOG_DEBUG_("[Date Response] retno=%d retmsg=%s\n", retno, retmsg.c_str());
	//这里当用户不在线或者是已经在棋桌的retno，在UserServer里面就直接处理了这两种情况
	if(retno < 0 || retno == 1)
	{
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
	int uid = inputPacket->ReadInt();
	int tid = inputPacket->ReadInt();
	short level = inputPacket->ReadShort();
	int64_t money = inputPacket->ReadInt64();

	GameTable *pTable = AllocManager::getInstance()->getAllocTable(level, tid, money);
	if(pTable == NULL)
	{
		_LOG_ERROR_("[ChangeTabProc] pTable is NULL uid[%d] level[%d]\n", uid, level);
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd, uid);
		ReportPkg.SetSeqNum(pt->seq);
		ReportPkg.WriteShort(-18);
		ReportPkg.WriteString(ErrorMsg::getInstance()->getErrMsg(-18));
		ReportPkg.End();
		if(pt->client)
		{
			this->send(pt->client, &ReportPkg, false);
		}
		return 0;
	}


	short clevel = Configure::instance()->m_nLevel;
	OutputPacket ReportPkg;
	ReportPkg.Begin(cmd, uid);
	ReportPkg.SetSeqNum(pt->seq);
	ReportPkg.WriteShort(0);
	ReportPkg.WriteString("ok");
	ReportPkg.WriteInt(uid);
	ReportPkg.WriteInt(pTable->m_nTid);
	ReportPkg.WriteShort(level);
	ReportPkg.End();

	_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", 0);
	_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", "ok");
	_LOG_DEBUG_("[DATA Reponse] uid=[%d] \n", uid);
	_LOG_DEBUG_("[DATA Reponse] m_nTid=[%d] \n", pTable->m_nTid);
	_LOG_DEBUG_("[DATA Response] level=[%d] \n", level);


	if(pt->client)
	{
		int n = this->send(pt->client, &ReportPkg, false);
		_LOG_DEBUG_("Send to Hall  success, len=[%d]\n", n);
	}
	else
		_LOG_DEBUG_("pt->client == NULL\n" );
	return 0;
}

