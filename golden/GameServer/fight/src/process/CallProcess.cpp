#include <string>
#include "CallProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_BET_CALL, CallProcess)

CallProcess::CallProcess()
{
	this->name = "CallProcess" ;
}

CallProcess::~CallProcess()
{
}

int CallProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int   cmd   = pPacket->GetCmdType();
	short seq   = pPacket->GetSeqNum();
	int   uid   = pPacket->GetUid();
	int   uid_b = pPacket->ReadInt();
	int   tid   = pPacket->ReadInt();
	int64_t betmoney = pPacket->ReadInt64();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	if (uid != uid_b) 
	{
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(clientHandler, cmd, uid, 4, _EMSG_(4), seq);
	}

	_LOG_DEBUG_("==>[CallProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	_LOG_DEBUG_("[DATA Parse] betmoney=[%ld] \n", betmoney);
	
	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);
	if (table == NULL)
	{
		_LOG_ERROR_("[CallProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if (player == NULL)
	{
		_LOG_ERROR_("[CallProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	if (!player->isActive())
	{
		_LOG_ERROR_("[CallProcess] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -11, _EMSG_(-11), seq);
	}

	if (table->m_nCurrBetUid != uid)
	{
		_LOG_ERROR_("[CallProcess] uid=[%d] tid=[%d] realTid=[%d] betUid[%d] is not this uid\n",uid, tid, realTid, table->m_nCurrBetUid);
		return sendErrorMsg(clientHandler, cmd, uid, -12, _EMSG_(-12), seq);
	}

	betmoney = Player::calculateCompMoney(player, table->m_BetMaxCardStatus, table->m_lCurrBetMax, table->m_nMaxLimit);
	
	//这里和下面表示此用户是Allin联系起来的
	if (table->hasSomeOneAllin())
	{
		betmoney = player->m_AllinCoin;
	}

	if(player->m_lMoney < betmoney)
	{
		_LOG_ERROR_("[CallProcess] uid=[%d] tid=[%d] realTid=[%d] player->m_lMoney=[%ld] is less bet money[%d] countmoney[%ld]\n",uid, tid, realTid, player->m_lMoney, betmoney, player->m_lSumBetCoin);
		return sendErrorMsg(clientHandler, cmd, uid, -4, _EMSG_(-4),seq);
	}

	if(table->m_bIsOverTimer)
	{
		_LOG_ERROR_("[CallProcess] table is IsOverTimer uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, 2, _EMSG_(2),seq);
	}

	if(table->hasSomeOneAllin())
	{
		player->m_bAllin = true;
	}

	table->playerBetCoin(player, betmoney);
	//说明此用户已经allin了
	/*if(player->m_lMoney <= 0)
	{
		player->m_bAllin = true;
	}*/
	
	_LOG_DEBUG_("[CallProcess] Uid=[%d] GameID=[%s] CallMoney=[%ld] m_lSumBetCoin=[%ld] currRound=[%d]\n", player->id, table->getGameID(), betmoney, player->m_lSumBetCoin, table->m_bCurrRound);
	
    player->recordCall(table->m_bCurrRound , betmoney);

	//设置下一个应该下注的用户
	Player* nextplayer = table->getNextBetPlayer(player, OP_CALL);

	player->setActiveTime(time(NULL));
	if(table->iscanGameOver())
		nextplayer = NULL;
	
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			sendCallInfoToPlayers(table->player_array[i], table, player, betmoney , nextplayer,seq);
		}
	}

	if(table->iscanGameOver())
		return table->gameOver();

	return 0;
}

int CallProcess::sendCallInfoToPlayers(Player* player, Table* table, Player* betcallplayer, int64_t betmoney,Player* nextplayer,short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_BET_CALL, player->id);
	if(player->id == betcallplayer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt(betcallplayer->id);
	response.WriteInt64(betmoney);
	response.WriteInt64(betcallplayer->m_lSumBetCoin);
	response.WriteInt64(betcallplayer->m_lMoney);
	response.WriteInt64(table->m_lCurrBetMax);
	response.WriteInt(nextplayer ? nextplayer->id : 0);
	response.WriteInt64(player->m_lSumBetCoin);
	response.WriteInt64(player->m_lMoney);
	response.WriteShort(player->optype);
	response.WriteInt64(table->m_lSumPool);
	response.WriteInt64(player->m_AllinCoin);
	response.WriteByte(player->m_bCardStatus);
	response.End();
	_LOG_DEBUG_("<==[CallProcess] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_BET_CALL, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus=[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_lCurrBetMax=[%d]\n", table->m_lCurrBetMax);
	_LOG_DEBUG_("[Data Response] betcallplayer_id=[%d]\n",betcallplayer->id);
	_LOG_DEBUG_("[Data Response] m_bCurrRound=[%d]\n", table->m_bCurrRound);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", player->optype);
	_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
	_LOG_DEBUG_("[Data Response] m_lSumBetCoin=[%d]\n", player->m_lSumBetCoin);
	_LOG_DEBUG_("[Data Response] m_lSumPool=[%d]\n", table->m_lSumPool);
	_LOG_DEBUG_("[Data Response] m_AllinCoin=[%ld]\n", player->m_AllinCoin);


	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[CallProcess] Send To Uid[%d] Error!\n", player->id);
	
	return 0;
}

int CallProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

