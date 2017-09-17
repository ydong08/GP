#include <string>
#include "LogoutProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "IProcess.h"
#include "GameCmd.h"
#include "ProcessManager.h"

using namespace std;


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
	short realTid = tid & 0x0000FFFF;

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[LogoutProcess] uid=[%d] tid=[%d] Table is NULL\n",uid, tid);
		return 0; 
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		//_LOG_ERROR_("[LogoutProcess] uid=[%d] tid=[%d] Your not in This Table\n",uid, tid);
		return 0;
	}

	player->isonline = false;
	ULOGGER(E_LOG_INFO, player->id) << "logout";
	if(player->notBetCoin() && table->bankeruid != player->id)
	{
		IProcess::serverPushLeaveInfo(table, player, 2);
		table->playerLeave(player);
	}
	return 0;
}


int LogoutProcess::sendPlayersLogoutInfo(Table* table, Player* logoutPlayer)
{
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_LOGOUT, table->player_array[i]->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(table->id);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(logoutPlayer->id);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] logoutPlayer=[%d]\n", logoutPlayer->id);
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_LOGOUT, LogoutProcess);
