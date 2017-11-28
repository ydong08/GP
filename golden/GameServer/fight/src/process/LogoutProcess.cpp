#include <string>
#include "LogoutProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "IProcess.h"

REGISTER_PROCESS(CLIENT_MSG_LOGOUT, LogoutProcess)

LogoutProcess::LogoutProcess()
{
	this->name = "LogoutProcess" ;
}

LogoutProcess::~LogoutProcess()
{
}

int LogoutProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{

	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();

	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[LogoutProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[LogoutProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return 0; 
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[LogoutProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return 0;
	}
	sendPlayersLogoutInfo(table, player);
	_LOG_WARN_("player[%d] m_nStatus[%d] logout\n", player->id, player->m_nStatus);

	player->isdropline = true;

	if(!player->isActive())
	{
		IProcess::serverPushLeaveInfo(table, player);
		table->playerLeave(player);
	}

	if(table->isAllReady() && table->m_nCountPlayer > 1)
	{
		return IProcess::GameStart(table);
	}

	return 0;
}

int LogoutProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);

	return 0;
}

int LogoutProcess::sendPlayersLogoutInfo(Table* table, Player* logoutPlayer)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	
	_LOG_DEBUG_("<==[LogoutProcess] Push [0x%04x]\n", CLIENT_MSG_LOGOUT);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int sendnum = 0;
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
		if (table->player_array[i] == NULL)
		{
			continue;
		}
		if(table->player_array[i]->id != logoutPlayer->id)
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_LOGOUT, table->player_array[i]->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(logoutPlayer->id);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
			sendnum++;
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] logoutPlayer=[%d]\n", logoutPlayer->id);
	return 0;
}
