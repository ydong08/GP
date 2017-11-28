#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "ClientNetStatus.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "IProcess.h"
#include "ProtocolClientId.h"

REGISTER_PROCESS(CLIENT_MSG_NETWORK_STATUS, ClientNetStatusProcess)

ClientNetStatusProcess::ClientNetStatusProcess()
{
	this->name = "ClientNetStatusProcess";
}

ClientNetStatusProcess::~ClientNetStatusProcess()
{
}

int ClientNetStatusProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	_NOTUSED(source);
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
    int signal = pPacket->ReadInt();

	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_DEBUG_("==>[ClientNetStatusProcess]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[ClientNetStatusProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[ClientNetStatusProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}
	_LOG_INFO_("[ClientNetStatusProcess] UID=[%d] tid=[%d] realTid=[%d] Leave OK\n", uid, tid, realTid);
	player->isdropline = true;
    IProcess::NotifyPlayerNetStatus(table, player->id, signal);
    return 0;
}

int ClientNetStatusProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

