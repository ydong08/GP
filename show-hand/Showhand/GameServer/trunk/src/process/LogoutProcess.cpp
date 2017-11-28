#include <string>
#include "LogoutProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "UdpManager.h"
#include "LogServerConnect.h"

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
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[LogoutProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);

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
	_LOG_WARN_("player[%d] status[%d] logout\n", player->id, player->status);

	if(!player->isActive())
	{
		IProcess::serverPushLeaveInfo(table, player);
		table->playerLeave(player);
	}
	else
	{
		Json::Value data;
		data["BID"] = string(table->getGameID());
		data["Time"]=(int)(time(NULL) - table->getStartTime());
		data["currd"] = table->currRound;
		data["logoutID"] = player->id;
		data["count"] = (double)player->betCoinList[0];
		if(!table->isAllRobot())
			_LOG_REPORT_(player->id, RECORD_LOG_OUT, "%s", data.toStyledString().c_str());
		data["errcode"] = 0;
		_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());
	}

	if(table->isAllReady() && table->countPlayer > 1)
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
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	
	_LOG_DEBUG_("<==[LogoutProcess] Push [0x%04x]\n", CLIENT_MSG_LOGOUT);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int sendnum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->countPlayer)
			break;
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_LOGOUT, table->player_array[i]->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->status);
			response.WriteInt(tid);
			response.WriteShort(table->status);
			response.WriteInt(logoutPlayer->id);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->hallid, &response, false);
			sendnum++;
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] logoutPlayer=[%d]\n", logoutPlayer->id);
	return 0;
}
