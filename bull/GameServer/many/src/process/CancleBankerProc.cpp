#include "CancleBankerProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include <json/json.h>
#include "ProcessManager.h"

CancleBankerProc::CancleBankerProc()
{
	this->name = "CancleBankerProc";
}

CancleBankerProc::~CancleBankerProc()
{

} 

int CancleBankerProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType() ;
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short sid = pPacket->ReadShort();

	_LOG_INFO_("==>[CancleBankerProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] sid=[%d]\n", sid);

	CDLSocketHandler* hallhandler = (clientHandler);

	Room* room = Room::getInstance();
    Table *table = room->getTable();
	int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("CancleBankerProc:table is NULL\n");
		sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("CancleBankerProc: uid[%d] Player is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		return 0;
	}

// 	if(player->id != uid)
// 	{
// 		_LOG_ERROR_("Your uid[%d] Not Set In this index sid[%d]\n", uid, sid);
// 		sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
// 		return 0;
// 	}

	if(!player->isbankerlist)
	{
		_LOG_ERROR_("Your uid[%d] Not In BankerList sid[%d]\n", uid, sid);
		sendErrorMsg(hallhandler, cmd, uid, -10,ErrorMsg::getInstance()->getErrMsg(-10),seq);
		return 0;
	}

	if(table->bankeruid == player->id && table->m_nStatus == STATUS_TABLE_IDLE)
	{
		_LOG_INFO_("Your uid[%d] is this table Banker sid[%d] m_nStatus is idle\n", uid, sid);
		OutputPacket response;
		response.Begin(cmd, player->id);
		response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(player->id);
		response.WriteShort(player->m_nStatus);
		response.WriteInt(table->id);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(player->id);
		response.End();
		if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
			_LOG_ERROR_("[CancleBankerProc] Send To Uid[%d] Error!\n", player->id);
		table->rotateBanker();
		return 0;
	}

	if(table->bankeruid == player->id)
	{
		_LOG_INFO_("Your uid[%d] is this table Banker sid[%d]\n", uid, sid);
		sendErrorMsg(hallhandler, cmd, uid, -28, ErrorMsg::getInstance()->getErrMsg(-28), seq);
		player->isCanlcebanker = true;
	}
	
	player->setActiveTime(time(NULL));
	table->CancleBanker(player);

	_LOG_DEBUG_("<==[CancleBankerProc] Push [0x%04x]\n", cmd);
	int sendnum = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, player, seq);
			sendnum++;
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] canlceid=[%d]\n", player->id);

	return 0;
}


int CancleBankerProc::sendTabePlayersInfo(Player* player, Table* table, Player* applyer, short seq)
{
	OutputPacket response;
	response.Begin(CLIENT_MSG_CANCLEBANKER, player->id);
	if(player->id == applyer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(applyer->id);
	response.End();
	_LOG_DEBUG_("[Data Response] push to uid=[%d] m_nStatus=[%d]\n", player->id,player->m_nStatus);
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[CancleBankerProc] Send To Uid[%d] Error!\n", player->id);
	//else
	//	_LOG_DEBUG_("[CancleBankerProc] Send To Uid[%d] Success\n", player->id);
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_CANCLEBANKER, CancleBankerProc)