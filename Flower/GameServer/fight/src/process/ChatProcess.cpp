#include "ChatProcess.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include <string>

#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_CHAT, ChatProcess)

struct Param
{
	int uid;
	int tid;
	int touid;
	short msgtype;
};


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
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	int touid = pPacket->ReadInt();
	BYTE type = pPacket->ReadByte();
	string msg = pPacket->ReadString();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[ChatProcess]  cmd[0x%04x] uid[%d] type[%d]\n", cmd, uid, type);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d] touid[%d]\n", tid, svid, realTid, touid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}
	Player* opponent = table->getPlayer(touid);
	if(touid != 0)
	{
		if(opponent == NULL)
		{
			_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] touid[%d] not in This Table\n",uid, tid, realTid, touid);
			return sendErrorMsg(clientHandler, cmd, uid, -10, _EMSG_(-10), seq);
		}
	}

	_LOG_INFO_("[ChatProcess] UID=[%d] tid=[%d] realTid=[%d] ChatProcess OK\n", uid, tid, realTid);

	player->setActiveTime(time(NULL));
	sendChatInfo(table, player, touid, type, msg.c_str(), seq);
	return 0;
}

int ChatProcess::sendChatInfo(Table* table, Player* player, int touid,  short type, const char* msg, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	_LOG_DEBUG_("<==[sendChatInfo] Push [0x%04x]\n", CLIENT_MSG_CHAT);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_CHAT,table->player_array[i]->id);
			if(table->player_array[i]->id == player->id)
				response.SetSeqNum(seq);
            response.WriteShort(0);
            response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
            response.WriteInt(tid);
            response.WriteShort(table->m_nStatus);
            response.WriteInt(player->id);
            response.WriteString(player->name);
            response.WriteString(player->json);
            response.WriteInt(touid);
            response.WriteByte(type);
            response.WriteString(msg);
            response.End();
            HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] ChatPlayer=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] touid=[%d]\n", touid);
	_LOG_DEBUG_("[Data Response] type=[%d]\n", type);
	_LOG_DEBUG_("[Data Response] m_lMoney=[%d]\n", player->m_lMoney);
	return 0;
}

int ChatProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	return 0;
}
