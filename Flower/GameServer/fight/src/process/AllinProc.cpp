#include <string>
#include "AllinProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_ALL_IN, AllinProc)

AllinProc::AllinProc()
{
	this->name = "AllinProc" ;
}

AllinProc::~AllinProc()
{
}

int AllinProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	int   cmd     = pPacket->GetCmdType();
	short seq     = pPacket->GetSeqNum();
	int   uid     = pPacket->GetUid();
	int   uid_b   = pPacket->ReadInt();
	int   tid     = pPacket->ReadInt();
	short svid    = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	if (uid != uid_b) 
	{
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(clientHandler, cmd, uid, 4, _EMSG_(4), seq);
	}

	_LOG_DEBUG_("==>[AllinProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);
	if (table == NULL)
	{
		_LOG_ERROR_("[AllinProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if (player == NULL)
	{
		_LOG_ERROR_("[AllinProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	if (!player->isActive())
	{
		_LOG_ERROR_("[AllinProc] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -11, _EMSG_(-11), seq);
	}

	if (table->m_nCurrBetUid != uid)
	{
		_LOG_ERROR_("[AllinProc] uid=[%d] tid=[%d] realTid=[%d] betUid[%d] is not this uid\n",uid, tid, realTid, table->m_nCurrBetUid);
		return sendErrorMsg(clientHandler, cmd, uid, -12, _EMSG_(-12), seq);
	}

	if (player->m_lMoney <= 0 && player->m_AllinCoin > 0)
	{
		_LOG_ERROR_("[AllinProc] uid=[%d] tid=[%d] realTid=[%d] player->m_lMoney=[%ld] countmoney[%ld]\n",uid, tid, realTid, player->m_lMoney, player->m_lSumBetCoin);
		return sendErrorMsg(clientHandler, cmd, uid, -4, _EMSG_(-4), seq);
	}

	if (table->m_bIsOverTimer)
	{
		_LOG_ERROR_("[AllinProc] table is IsOverTimer uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, 2, _EMSG_(2), seq);
	}

	int64_t betmoney = player->m_AllinCoin;//table->FormatValue(player->m_bCardStatus, player->m_AllinCoin, table->m_BetMaxCardStatus);
	if (betmoney > table->m_lCurrBetMax)
	{
		table->m_lCurrBetMax = player->m_AllinCoin;
		table->m_BetMaxCardStatus = player->m_bCardStatus;
		table->NotifyUnknowCardMaxNum(NULL);
	}
	
	table->m_lHasAllInCoin = player->m_AllinCoin;
	player->m_bAllin = true;
	player->setActiveTime(time(NULL));
	table->playerBetCoin(player, betmoney);

	_LOG_INFO_("[AllinProc] Uid=[%d] GameID=[%s] AllinMoney=[%ld] BetCountMoney=[%ld] currRound=[%d]\n", player->id, table->getGameID(), betmoney, player->m_lSumBetCoin, table->m_bCurrRound);

    player->recordAllIn(table->m_bCurrRound , betmoney);
	//设置下一个应该下注的用户
	Player* nextplayer = table->getNextBetPlayer(player, OP_ALLIN);

	if(table->iscanGameOver())
		nextplayer = NULL;
	
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			sendAllinInfoToPlayers(table->player_array[i], table, player, betmoney , nextplayer,seq);
		}
	}

	if(table->iscanGameOver())
		return table->gameOver();
	return 0;
}

int AllinProc::sendAllinInfoToPlayers(Player* player, Table* table, Player* allinplayer, int64_t betmoney,Player* nextplayer, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_ALL_IN, player->id);
	if(player->id == allinplayer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt(allinplayer->id);
	response.WriteInt64(betmoney);
	response.WriteInt64(allinplayer->m_lSumBetCoin);
	response.WriteInt64(allinplayer->m_lMoney);
	response.WriteInt64(table->m_lCurrBetMax);
	response.WriteInt(nextplayer ? nextplayer->id : 0);
	response.WriteInt64(player->m_lSumBetCoin);
	response.WriteInt64(player->m_lMoney);
	response.WriteShort(player->optype);
	response.WriteInt64(table->m_lSumPool);
	response.WriteInt64(player->m_AllinCoin);
	response.WriteByte(player->m_bCardStatus);
	response.End();
	_LOG_DEBUG_("<==[AllinProc] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_ALL_IN, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus=[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_lCurrBetMax=[%d]\n", table->m_lCurrBetMax);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", player->optype);
	_LOG_DEBUG_("[Data Response] m_bCurrRound=[%d]\n", table->m_bCurrRound);
	_LOG_DEBUG_("[Data Response] m_lMoney=[%d]\n", player->m_lMoney);
	_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
	_LOG_DEBUG_("[Data Response] m_AllinCoin=[%d]\n", player->m_AllinCoin);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[AllinProc] Send To Uid[%d] Error!\n", player->id);
	
	return 0;
}

int AllinProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

