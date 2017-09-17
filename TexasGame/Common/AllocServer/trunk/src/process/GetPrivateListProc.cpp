#include "GetPrivateListProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "AllocManager.h"
#include "Configure.h"
#include "ErrorMsg.h"

GetPrivateListProc::GetPrivateListProc()
{
	this->name = "GetPrivateListProc";
}

GetPrivateListProc::~GetPrivateListProc()
{

} 

int GetPrivateListProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	short level = pPacket->ReadShort();

	_LOG_INFO_("==>[GetPrivateListProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] level=[%d] \n", level);

	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	if(level != Configure::instance()->m_nLevel)
	{
		_LOG_ERROR_("[GetPrivateListProc] Level[%d] is not equal AllocLevel[%d]\n", level, Configure::instance()->m_nLevel);
		return sendErrorMsg(clientHandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
	}


	TableListMap list;
	AllocManager::getInstance()->getPrivateList(list);

	OutputPacket ReportPkg;
	ReportPkg.Begin(cmd, uid);
	ReportPkg.SetSeqNum(seq);
	ReportPkg.WriteShort(0);
	ReportPkg.WriteString("ok");
	ReportPkg.WriteInt(uid);
	ReportPkg.WriteShort((short)list.size());
	TableListMap::iterator iterTable = list.begin();
	for(; iterTable!=list.end(); iterTable++)
	{
		GameTable *pTable = iterTable->second;
		if(pTable != NULL)
		{
			ReportPkg.WriteInt(pTable->m_nTid);
			ReportPkg.WriteShort(pTable->m_nUserCount);
			ReportPkg.WriteShort(pTable->m_MaxCount);
			ReportPkg.WriteShort(pTable->m_bhaspasswd ? 1 : 0);
			ReportPkg.WriteString(pTable->m_szName);
		}
	}
	ReportPkg.End();
	

	_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", 0);
	_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", "ok");
	_LOG_DEBUG_("[DATA Reponse] uid=[%d] \n", uid);
	_LOG_DEBUG_("[DATA Reponse] listsize=[%d] \n", list.size());
	this->send(clientHandler, &ReportPkg, false);
	
	return 0;
}

int GetPrivateListProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

