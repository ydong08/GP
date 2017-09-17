#include <string>
#include "ThrowCardProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "ProcessManager.h"
#include "IProcess.h"
#include "BaseClientHandler.h"
#include "ProtocolServerId.h"

REGISTER_PROCESS(CLIENT_MSG_THROW_CARD, ThrowCardProc)

ThrowCardProc::ThrowCardProc()
{
	this->name = "ThrowCardProc" ;
}

ThrowCardProc::~ThrowCardProc()
{
}

int ThrowCardProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int   uid   = pPacket->GetUid();
	int   uid_b = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	if (uid != uid_b) {
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(hallhandler, cmd, uid, 4, _EMSG_(4),seq);
	}

	_LOG_DEBUG_("==>[ThrowCardProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,_EMSG_(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,_EMSG_(-1),seq);
	}

	if( ! player->isActive())
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(hallhandler, cmd, uid, -11,_EMSG_(-11),seq);
	}

	if(player->m_bCardStatus == CARD_DISCARD)
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] your cardstatus[%d] no card\n", uid, tid, realTid, table->m_nStatus, player->m_bCardStatus);
		return sendErrorMsg(hallhandler, cmd, uid, -21,_EMSG_(-21),seq);
	}

	if(table->m_bIsOverTimer)
	{
		_LOG_ERROR_("[ThrowCardProc] table is IsOverTimer uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(hallhandler, cmd, uid, 2,_EMSG_(2),seq);
	}

	_LOG_INFO_("[ThrowCardProc] Uid=[%d] GameID=[%s] m_lSumBetCoin=[%ld] currRound=[%d]\n", player->id, table->getGameID(), player->m_lSumBetCoin, table->m_bCurrRound);

    player->recordThrowCard(table->m_bCurrRound , player->m_lSumBetCoin);

	player->m_bCardStatus = CARD_DISCARD;
    player->is_discard = true;

	player->setActiveTime(time(NULL));
	Player* nextplayer = NULL;
	if(table->m_nCurrBetUid == player->id)
	{
		nextplayer = table->getNextBetPlayer(player, OP_THROW);
	}
	else
	{
		//处理当前不是此用户下注的时候弃牌，然后操作类型的问题
		Player* currenter = table->getPlayer(table->m_nCurrBetUid);
		table->setPlayerOptype(currenter, 0);
	}

	if(player->m_nTabIndex == table->m_nFirstIndex)
	{
		table->setNextFirstPlayer(player);
	}
	if(table->iscanGameOver())
		nextplayer = NULL;
	for(int i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			sendThrowInfoToPlayers(table->player_array[i], table, player, nextplayer,seq);
		}
	}
	if(table->iscanGameOver())
		return table->gameOver();

	return 0;
}


int ThrowCardProc::sendThrowInfoToPlayers(Player* player, Table* table, Player* throwcallplayer,Player* nextplayer, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_THROW_CARD, player->id);
	if(player->id == throwcallplayer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt(throwcallplayer->id);
	response.WriteInt64(throwcallplayer->m_lSumBetCoin);
	response.WriteInt64(throwcallplayer->m_lMoney);
	response.WriteInt64(table->m_lCurrBetMax);
	response.WriteInt(nextplayer ? nextplayer->id : 0);
	response.WriteInt64(player->m_lSumBetCoin);
	response.WriteInt64(player->m_lMoney);
	response.WriteShort(player->optype);
	response.WriteInt64(table->m_lSumPool);
	response.WriteInt64(player->m_AllinCoin);
	response.WriteByte(player->m_bCardStatus);
	response.End();
	_LOG_DEBUG_("<==[ThrowCardProc] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_THROW_CARD, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus=[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] throwcallplayer=[%d]\n", throwcallplayer->id);
	_LOG_DEBUG_("[Data Response] m_bCurrRound=[%d]\n", table->m_bCurrRound);
	_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", player->optype);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[BetCallProc] Send To Uid[%d] Error!\n", player->id);
	return 0;
}

int ThrowCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

