#include "CreatePrivateProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "AllocManager.h"
#include "Configure.h"
#include "ServerManager.h"
#include "ErrorMsg.h"
#include <string>
using namespace std;

CreatePrivateProc::CreatePrivateProc()
{
	this->name = "CreatePrivateProc";
}

CreatePrivateProc::~CreatePrivateProc()
{

} 

int CreatePrivateProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	short level = pPacket->ReadShort();
	short maxcount = pPacket->ReadShort();
	string tablename = pPacket->ReadString();
	short haspasswd = pPacket->ReadShort();
	string passwrod = pPacket->ReadString();
	int64_t money = pPacket->ReadInt64();

	_LOG_INFO_("==>[CreatePrivateProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] level=[%d] \n", level);
	_LOG_DEBUG_("[DATA Parse] maxcount=[%d] \n", maxcount);
	_LOG_DEBUG_("[DATA Parse] tablename=[%s] \n", tablename.c_str());
	_LOG_DEBUG_("[DATA Parse] haspasswd=[%d] \n", haspasswd);
	_LOG_DEBUG_("[DATA Parse] passwrod=[%s] \n", passwrod.c_str());

	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	if(level != Configure::instance()->m_nLevel)
	{
		_LOG_ERROR_("uid[%d] Level[%d] is not equal AllocLevel[%d]\n",uid, level, Configure::instance()->m_nLevel);
		return sendErrorMsg(clientHandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
	}
	if(tablename.size() < 1)
	{
		_LOG_ERROR_("uid[%d] Level[%d] tablename[%s] is NULL\n",uid, level,tablename.c_str());
		return sendErrorMsg(clientHandler, cmd, uid, -20,ErrorMsg::getInstance()->getErrMsg(-20),seq);
	}
	if(money <= 0)
	{
		_LOG_ERROR_("uid[%d] Level[%d] not more money[%d]\n",uid, level,money);
		return sendErrorMsg(clientHandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),seq);
	}

	if(maxcount > 4 || maxcount < 2)
	{
		_LOG_ERROR_("uid[%d] maxcount[%d] is Error\n", uid, maxcount);
		return sendErrorMsg(clientHandler, cmd, uid, -20,ErrorMsg::getInstance()->getErrMsg(-20),seq);
	}

	GameTable *pTable = AllocManager::getInstance()->createPrivateTable(level);
	if(pTable == NULL)
	{
		_LOG_ERROR_("[CreatePrivateProc] pTable is NULL uid[%d] level[%d]\n", uid, level);
		return sendErrorMsg(clientHandler, cmd, uid, -7,ErrorMsg::getInstance()->getErrMsg(-7),seq);
	}
	pTable->m_nOwner = uid;
	pTable->m_bhaspasswd = (haspasswd==1) ? true : false;
	strncpy(pTable->m_szName, tablename.c_str(), sizeof(pTable->m_szName));
	pTable->m_szName[sizeof(pTable->m_szName) - 1] = '\0';
	pTable->m_MaxCount = maxcount;

	int svid = pTable->m_nTid>>16;
	OutputPacket ReportPkg1;
	ReportPkg1.Begin(cmd, uid); 
	ReportPkg1.WriteInt(uid);
	ReportPkg1.WriteInt(pTable->m_nTid);
	//ReportPkg1.WriteShort(pTable->m_MaxCount);
	ReportPkg1.WriteString(pTable->m_szName);
	ReportPkg1.WriteShort(haspasswd);
	ReportPkg1.WriteString(passwrod);
	ReportPkg1.End();

	int n = ServerManager::getInstance()->sendToGmServer(svid, &ReportPkg1, false); 
	_LOG_DEBUG_("sendToGmServer svid[%d] n=[%d]\n",svid, n);

	OutputPacket ReportPkg;
	ReportPkg.Begin(cmd, uid);
	ReportPkg.SetSeqNum(seq);
	ReportPkg.WriteShort(0);
	ReportPkg.WriteString("ok");
	ReportPkg.WriteInt(uid);
	ReportPkg.WriteInt(pTable->m_nTid);
	ReportPkg.WriteShort(Configure::instance()->m_nLevel);
	ReportPkg.End();

	_LOG_DEBUG_("[DATA Reponse] erron=[%d] errmsg=[%s] \n", 0,"ok");
	_LOG_DEBUG_("[DATA Reponse] uid=[%d] \n", uid);
	_LOG_DEBUG_("[DATA Reponse] m_nTid=[%d] \n", pTable->m_nTid);
	_LOG_DEBUG_("[DATA Response] level=[%d] \n", Configure::instance()->m_nLevel);
	this->send(clientHandler, &ReportPkg, false);
	
	return 0;
}

int CreatePrivateProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

