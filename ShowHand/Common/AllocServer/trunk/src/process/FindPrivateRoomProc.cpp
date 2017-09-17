#include "FindPrivateRoomProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "AllocManager.h"
#include "Configure.h"
#include "ErrorMsg.h"

FindPrivateRoomProc::FindPrivateRoomProc()
{
	this->name = "FindPrivateRoomProc";
}

FindPrivateRoomProc::~FindPrivateRoomProc()
{

} 

int FindPrivateRoomProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	short level = pPacket->ReadShort();
	int tid = pPacket->ReadInt();

	_LOG_INFO_("==>[FindPrivateRoomProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] level=[%d] tid=[%d]\n", level, tid);

	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	if(level != Configure::instance()->m_nLevel)
	{
		_LOG_ERROR_("Level[%d] is not equal AllocLevel[%d]\n", level, Configure::instance()->m_nLevel);
		return sendErrorMsg(clientHandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
	}

	GameTable *pTable = AllocManager::getInstance()->getTable(tid);

	if(pTable == NULL)
	{
		_LOG_ERROR_("[FindPrivateRoomProc] pTable is NULL uid[%d] tid[%d] level[%d]\n", uid, tid, level);
		return sendErrorMsg(clientHandler, cmd, uid, -15,ErrorMsg::getInstance()->getErrMsg(-15),seq);
	}

	if(pTable->m_nUserCount <= 0)
	{
		_LOG_ERROR_("[FindPrivateRoomProc] pTable m_nUserCount[%d] <=0 uid[%d] tid[%d] level[%d]\n", pTable->m_nUserCount, uid, tid, level);
		return sendErrorMsg(clientHandler, cmd, uid, -15,ErrorMsg::getInstance()->getErrMsg(-15),seq);
	}

	OutputPacket ReportPkg;
	ReportPkg.Begin(cmd, uid);
	ReportPkg.SetSeqNum(seq);
	ReportPkg.WriteShort(0);
	ReportPkg.WriteString("ok");
	ReportPkg.WriteInt(uid);
	ReportPkg.WriteInt(pTable->m_nTid);
	ReportPkg.WriteShort(pTable->m_nUserCount);
	ReportPkg.WriteShort(pTable->m_MaxCount);
	ReportPkg.WriteShort(pTable->m_bhaspasswd ? 1 : 0);
	ReportPkg.WriteString(pTable->m_szName);
	ReportPkg.End();
	

	_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", 0);
	_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", "ok");
	_LOG_DEBUG_("[DATA Reponse] uid=[%d] \n", uid);
	_LOG_DEBUG_("[DATA Reponse] m_nTid=[%d] \n", pTable->m_nTid);
	_LOG_DEBUG_("[DATA Reponse] m_nUserCount=[%d] \n", pTable->m_nUserCount);
	_LOG_DEBUG_("[DATA Reponse] m_MaxCount=[%d] \n", pTable->m_MaxCount);
	_LOG_DEBUG_("[DATA Reponse] m_bhaspasswd=[%d] \n", pTable->m_bhaspasswd ? 1 : 0);
	_LOG_DEBUG_("[DATA Reponse] m_szName=[%s] \n", pTable->m_szName);
	this->send(clientHandler, &ReportPkg, false);
	
	return 0;
}

int FindPrivateRoomProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

