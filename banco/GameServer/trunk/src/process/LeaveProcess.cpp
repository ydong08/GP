#include "LeaveProcess.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"

LeaveProcess::LeaveProcess()
{
	this->name = "LeaveProcess";
}

LeaveProcess::~LeaveProcess()
{
} 

int LeaveProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

    if (table->bankeruid == player->id)
    {
        ULOGGER(E_LOG_INFO, player->id) << "banker can't leave right now, banker_id = " << table->bankeruid;
        return sendErrorMsg(clientHandler, cmd, uid, -45, ErrorMsg::getInstance()->getErrMsg(-45), seq);
    }

	sendLeaveInfo(table, player, seq);
	table->playerLeave(player);

	return 0;
}

int LeaveProcess::sendLeaveInfo(Table* table, Player* leavePlayer, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player *current = table->player_array[i];
		if (current == NULL)
		{
			continue;
		}

		OutputPacket response;
		response.Begin(CLIENT_MSG_LEAVE, current->id);
		if(leavePlayer->id == current->id)
			response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(current->id);
		response.WriteShort(current->m_nStatus);
		response.WriteInt(tid);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(leavePlayer->id);
		response.End();

		HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);

		LOGGER(E_LOG_INFO) << " cmd = " << TOHEX(CLIENT_MSG_LEAVE)
			<< " dest_uid = " << current->id
			<< " table_id = " << table->id
			<< " leave_uid = " << leavePlayer->id
			<< " seq = " << seq;
	}
	return 0;
}


REGISTER_PROCESS(CLIENT_MSG_LEAVE, LeaveProcess);
