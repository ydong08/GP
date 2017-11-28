#include <string>
#include "CheckProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "CoinConf.h"

REGISTER_PROCESS(CLIENT_MSG_CHECK_CARD, CheckProcess)

CheckProcess::CheckProcess()
{
	this->name = "CheckProcess" ;
}

CheckProcess::~CheckProcess()
{
}

int CheckProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int   cmd   = pPacket->GetCmdType();
	short seq   = pPacket->GetSeqNum();
	int   uid   = pPacket->GetUid();
	int   uid_b = pPacket->ReadInt();
	int   tid   = pPacket->ReadInt();
	short svid  = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_DEBUG_("==>[CheckProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	if (uid != uid_b) {
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(clientHandler, cmd, uid, 4, _EMSG_(4), seq);
	}

	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[CheckProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[CheckProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	if( !player->isActive())
	{
		_LOG_ERROR_("[CheckProcess] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n", uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -11, _EMSG_(-11), seq);
	}
	//第一轮比牌不能看牌
	if (table->m_bCurrRound <= CoinConf::getInstance()->getCoinCfg()->check_round)
	{
		_LOG_ERROR_("[CheckProcess] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not check\n", uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -34, _EMSG_(-34), seq);
	}
	
	if(player->m_bCardStatus != CARD_UNKNOWN)
	{
		_LOG_ERROR_("[CheckProcess] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] your cardstatus[%d] not unknown\n", uid, tid, realTid, table->m_nStatus, player->m_bCardStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -21, _EMSG_(-21), seq);
	}

	if(table->m_bIsOverTimer)
	{
		_LOG_ERROR_("[CheckProcess] table is IsOverTimer uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -35, _EMSG_(-35), seq);
	}

	_LOG_INFO_("[CheckProcess] Uid=[%d] GameID=[%s] m_lSumBetCoin=[%ld] currRound=[%d]\n", player->id, table->getGameID(), player->m_lSumBetCoin, table->m_bCurrRound);

    player->recordCheck(table->m_bCurrRound , player->m_lSumBetCoin);
	
	//设置下一个应该下注的用户
	player->m_bCardStatus = CARD_KNOWN;

	if(player->id == table->m_nCurrBetUid)
	{
		table->stopBetCoinTimer();
		table->startBetCoinTimer(player->id,Configure::getInstance()->betcointime);
		player->setBetCoinTime(time(NULL));
		table->setPlayerOptype(player,0);
	}
	else
		player->optype &= 0x0F77; //剔除掉看牌的操作

	table->NotifyUnknowCardMaxNum(player);

	for(int i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			sendCheckInfoToPlayers(table->player_array[i], table, player,seq);
		}
	}
	
	return 0;
}

int CheckProcess::sendCheckInfoToPlayers(Player* player, Table* table, Player* checkplayer, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_CHECK_CARD, player->id);
	if(player->id == checkplayer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt(checkplayer->id);
	if(player->id == checkplayer->id)
	{
		response.WriteByte(player->card_array[0]);
		response.WriteByte(player->card_array[1]);
		response.WriteByte(player->card_array[2]);
		response.WriteByte(player->m_nEndCardType);
	}
	else
	{
		response.WriteByte(0);
		response.WriteByte(0);
		response.WriteByte(0);
		response.WriteByte(0);
	}
	response.WriteByte(player->m_bCardStatus);
	response.WriteShort(player->optype);
	if (player->id == checkplayer->id)
	{
		response.WriteInt(table->m_nRase1);
		response.WriteInt(table->m_nRase2);
		response.WriteInt(table->m_nRase3);
		response.WriteInt(table->m_nRase4);
		response.WriteInt(table->m_nMaxLimit);
	}
	else
	{
		response.WriteInt(0);
		response.WriteInt(0);
		response.WriteInt(0);
		response.WriteInt(0);
		response.WriteInt(0);
	}
	response.End();
	_LOG_DEBUG_("<==[CheckProcess] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_CHECK_CARD, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus=[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] checkplayer=[%d]\n", checkplayer->id);
	_LOG_DEBUG_("[Data Response] m_bCurrRound=[%d]\n", table->m_bCurrRound);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", player->optype);
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[BetCallProc] Send To Uid[%d] Error!\n", player->id);
	
	return 0;
}

int CheckProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

