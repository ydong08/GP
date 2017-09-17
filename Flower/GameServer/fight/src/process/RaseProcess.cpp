#include <string>
#include "RaseProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "ProcessManager.h"
#include "IProcess.h"

REGISTER_PROCESS(CLIENT_MSG_BET_RASE, RaseProcess)
REGISTER_PROCESS(CLIENT_MSG_BET_RASE_FOR_JS, RaseProcess)

RaseProcess::RaseProcess()
{
	this->name = "RaseProcess" ;
}

RaseProcess::~RaseProcess()
{
}

int RaseProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int   uid   = pPacket->GetUid();
	int   uid_b = pPacket->ReadInt();
	int   tid = pPacket->ReadInt();
	int64_t betmoney = pPacket->ReadInt64();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	if (uid != uid_b) {
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(hallhandler, cmd, uid, 4, _EMSG_(4),seq);
	}

	_LOG_INFO_("==>[RaseProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	_LOG_DEBUG_("[DATA Parse] betmoney=[%ld] \n", betmoney);

	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[RaseProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[RaseProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	if(!player->isActive())
	{
		_LOG_ERROR_("[RaseProcess] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(hallhandler, cmd, uid, -11, _EMSG_(-11), seq);
	}

	if(table->m_nCurrBetUid != uid)
	{
		_LOG_ERROR_("[RaseProcess] uid=[%d] tid=[%d] realTid=[%d] betUid[%d] is not this uid\n",uid, tid, realTid, table->m_nCurrBetUid);
		return sendErrorMsg(hallhandler, cmd, uid, -12, _EMSG_(-12), seq);
	}

	int64_t curMoney = table->FormatValue(player->m_bCardStatus, betmoney, table->m_BetMaxCardStatus);
	if(table->m_lCurrBetMax > curMoney)
	{
		_LOG_ERROR_("[RaseProcess] uid=[%d] tid=[%d] realTid=[%d] player->m_lMoney=[%ld] is less bet money:%d[%ld] m_lCurrBetMax:%d[%ld]\n",uid, tid, realTid, player->m_lMoney, player->m_bCardStatus, betmoney, table->m_lCurrBetMax, table->m_BetMaxCardStatus);
		return sendErrorMsg(hallhandler, cmd, uid, -26, _EMSG_(-26), seq);
	}

	int64_t raseMoney = 0;
	if(player->m_bCardStatus == CARD_UNKNOWN)
		raseMoney = betmoney;
	else
	{
		if (betmoney == 20)
		{
			raseMoney = 50;
		}
		else
		{
			raseMoney = betmoney * 2;
		}
	}
		
	if(player->m_lMoney < raseMoney)
	{
		_LOG_ERROR_("[RaseProcess] uid=[%d] tid=[%d] realTid=[%d] player->m_lMoney=[%ld] is less bet money[%d] countmoney[%ld]\n",uid, tid, realTid, player->m_lMoney, raseMoney, player->m_lSumBetCoin + raseMoney);
		return sendErrorMsg(hallhandler, cmd, uid, -4,_EMSG_(-4),seq);
	}

	if(table->m_bIsOverTimer)
	{
		_LOG_ERROR_("[RaseProcess] table is IsOverTimer uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(hallhandler, cmd, uid, 2,_EMSG_(2),seq);
	}

	table->m_lCurrBetMax = betmoney;
	table->m_nRaseUid = uid;
	table->m_BetMaxCardStatus = player->m_bCardStatus;
	table->NotifyUnknowCardMaxNum(NULL);

	table->playerBetCoin(player, raseMoney);

	_LOG_INFO_("[RaseProcess] Uid=[%d] GameID=[%s] raseMoney=[%ld] m_lSumBetCoin=[%ld] currRound=[%d]\n", player->id, table->getGameID(), raseMoney, player->m_lSumBetCoin, table->m_bCurrRound);

    player->recordRaise(table->m_bCurrRound , raseMoney);

	/*if(player->m_lMoney <= 0)
	{
		player->m_bAllin = true;
	}*/

	//设置下一个应该下注的用户
	Player* nextplayer = table->getNextBetPlayer(player, OP_RASE);
	
	player->setActiveTime(time(NULL));
	
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			sendRaseInfoToPlayers(table->player_array[i], cmd, table, player, raseMoney , nextplayer,seq);
		}
	}
	
	return 0;
}

int RaseProcess::sendRaseInfoToPlayers(Player* player, int cmd, Table* table, Player* raseplayer,int64_t betmoney,Player* nextplayer,short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(cmd, player->id);
	if(player->id == raseplayer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt(raseplayer->id);
	response.WriteInt64(betmoney);
	response.WriteInt64(raseplayer->m_lSumBetCoin);
	response.WriteInt64(raseplayer->m_lMoney);
	response.WriteInt64(table->m_lCurrBetMax);
	response.WriteInt(nextplayer ? nextplayer->id : 0);
	response.WriteInt64(player->m_lSumBetCoin);
	response.WriteInt64(player->m_lMoney);
	response.WriteShort(player->optype);
	response.WriteInt64(table->m_lSumPool);
	response.WriteInt64(player->m_AllinCoin);
	response.WriteByte(player->m_bCardStatus);
	response.End();
	_LOG_DEBUG_("<==[RaseProcess] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_BET_RASE, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus=[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_lCurrBetMax=[%d]\n", table->m_lCurrBetMax);
	_LOG_DEBUG_("[Data Response] raseplayer=[%d]\n",raseplayer->id);
	_LOG_DEBUG_("[Data Response] m_bCurrRound=[%d]\n", table->m_bCurrRound);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", player->optype);
	_LOG_DEBUG_("[Data Response] m_lMoney=[%ld]\n", player->m_lMoney);
	_LOG_DEBUG_("[Data Response] m_AllinCoin=[%ld]\n", player->m_AllinCoin);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[BetCallProc] Send To Uid[%d] Error!\n", player->id);
	
	return 0;
}

int RaseProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

