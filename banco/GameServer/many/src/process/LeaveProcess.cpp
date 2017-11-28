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

	Table* table = room->getTable();

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
	if(player->notBetCoin() && table->bankeruid != player->id)
	{
		ULOGGER(E_LOG_INFO, player->id) << "player leave now";
		OutputPacket response;
		response.Begin(cmd,player->id);
		response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(player->id);
		response.WriteShort(player->m_nStatus);
		response.WriteInt(table->id);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(player->id);
		response.End();
		HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);

		table->playerLeave(player);
	}
	else
	{
		player->isonline = false;
		ULOGGER(E_LOG_INFO, player->id) << "player cant leave";
		//_LOG_DEBUG_("[Data Response] player=[%d] leave error logout\n", player->id);
	}
	return 0;
}

int LeaveProcess::sendLeaveInfo(Table* table, Player* leavePlayer, short seq)
{
	int sendnum = 0;
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_LEAVE,table->player_array[i]->id);
			if(leavePlayer->id == table->player_array[i]->id)
				response.SetSeqNum(seq);
			response.WriteShort(0);
			response.WriteString("ok");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(leavePlayer->id);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
			sendnum++;
		}
	}
	return 0;
}


REGISTER_PROCESS(CLIENT_MSG_LEAVE, LeaveProcess);
