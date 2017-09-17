#include <string>
#include "LogChangeProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "MoneyAgent.h"
#include "BaseClientHandler.h"
#include "Player.h"
#include "Table.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "ProtocolServerId.h"

using namespace std;

REGISTER_PROCESS(CLIENT_MSG_LOGINCHANGE, LogChangeProc)

LogChangeProc::LogChangeProc()
{
	this->name = "LogChangeProc";
}

LogChangeProc::~LogChangeProc()
{

} 

int LogChangeProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short level = pPacket->ReadShort();
	int64_t money = pPacket->ReadInt64();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;

	LOGGER(E_LOG_DEBUG) << "LogChangeProc: params ="
		<< " cmd = " << TOHEX(cmd)
		<< " seq = " << seq
		<< " uid = " << uid
		<< " tid = " << tid
		<< " svid = " << tid
		<< " realTid = " << realTid;

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Table* t = Room::getInstance()->getTable(realTid);
	if (t == NULL)
	{
		ULOGGER(E_LOG_ERROR, uid) << "tid = " << tid << " real tid = " << realTid << " table is NULL";
		return sendErrorMsg(hallhandler, cmd, uid, -2, seq);
	}

	Player* p = Room::getInstance()->getPlayerUID(uid);
	if (p == NULL)
	{
		ULOGGER(E_LOG_ERROR, uid) << "tid = " << tid << " real tid = " << realTid << " Your not in This Table";
		return sendErrorMsg(hallhandler, cmd, uid, -1, seq);
	}

	if (t->bankeruid == p->id)
	{
		return sendErrorMsg(hallhandler, cmd, uid, -34, ERRMSG(-34), seq);
	}

	t->playerLeave(p);

	t = Room::getInstance()->getChangeTable(realTid);
	if (NULL == t)
	{
		return sendErrorMsg(hallhandler, cmd, uid, -18, ERRMSG(-18), seq);
	}

	// 等客户端主动发送0x200消息
	//t->playerComming(p);

	OutputPacket response;
	{
		int packedTid = Configure::getInstance()->m_nServerId << 16 | t->id;

		response.Begin(cmd, uid);
		response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(uid);
		response.WriteInt(packedTid);
		response.WriteShort(level);

		ULOGGER(E_LOG_INFO, uid) << "Change table success, cmd = " << TOHEX(cmd)
			<< " tid = " << t->id
			<< " packedTid = " << packedTid;
	}
	response.End();

	if (HallManager::getInstance()->sendToHall(hallhandler->hallid, &response, false) < 0)
	{
		ULOGGER(E_LOG_ERROR, uid) << "Change table failed!";
	}
	else
	{
		ULOGGER(E_LOG_ERROR, uid) << "Change table successed!";
	}

	return 0;
}

int LogChangeProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
