#include "ChatProcess.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "ProcessManager.h"
#include "ProtocolServerId.h"
#include "GameCmd.h"
#include <string>
#include "IProcess.h"
#include "Player.h" 
#include "Table.h"

using namespace std;

ChatProcess::ChatProcess()
{
	this->name = "ChatProcess";
}

ChatProcess::~ChatProcess()
{

} 

int ChatProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	_NOTUSED(source);
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	int touid = pPacket->ReadInt();
	BYTE type = pPacket->ReadByte();
	string msg = pPacket->ReadString();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;

	ULOGGER(E_LOG_DEBUG, uid) << "chat_proc_params ="
		<< " cmd = " << TOHEX(cmd)
		<< " seq = " << seq
		<< " from_uid = " << uid
		<< " tid = " << tid
		<< " to_uid = " << touid
		<< " type = " << type
		<< " msg = " << msg
		<< " svid = " << svid
		<< " realTid = " << realTid;

	Table* table = Room::getInstance()->getTable(realTid);
	if (table == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n", uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, ErrorMsg::getInstance()->getErrMsg(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if (player == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n", uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, ErrorMsg::getInstance()->getErrMsg(-1), seq);
	}

	Player* opponent = table->getPlayer(touid);
	if (touid != 0)
	{
		if (opponent == NULL)
		{
			_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] touid[%d] not in This Table\n", uid, tid, realTid, touid);
			return sendErrorMsg(clientHandler, cmd, uid, -22, ErrorMsg::getInstance()->getErrMsg(-22), seq);
		}
	}

	player->setActiveTime(time(NULL));

	IProcess::sendChatInfo(table, player, touid, type, msg.c_str(), seq);

	return 0;
}

int ChatProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_CHAT, ChatProcess)