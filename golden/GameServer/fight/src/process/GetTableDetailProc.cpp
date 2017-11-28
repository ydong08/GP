#include <string>
#include "GetTableDetailProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "BaseClientHandler.h"

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetTableDetailProc)

GetTableDetailProc::GetTableDetailProc()
{
	this->name = "GetTableDetailProc" ;
}

GetTableDetailProc::~GetTableDetailProc()
{
}

int GetTableDetailProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int   cmd   = pPacket->GetCmdType();
	short seq   = pPacket->GetSeqNum();
	int   uid   = pPacket->GetUid();
	int   uid_b = pPacket->ReadInt();
	int	  tid   = pPacket->ReadInt();
	short svid  = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	if (uid != uid_b) {
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(clientHandler, cmd, uid, 4, _EMSG_(4), seq);
	}

	_LOG_INFO_("==>[GetTableDetailProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		//用户离开，告诉userserver
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		//用户离开，告诉userserver
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	player->m_nHallid = hallhandler->hallid;
	player->isdropline = false;
	int i= 0;
	Player* betplayer = table->getPlayer(table->m_nCurrBetUid);
	short remaintime = 0;
	//当前超时时间
	if(betplayer)
		remaintime = Configure::getInstance()->betcointime - (time(NULL) - betplayer->GetBetCoinTime());
	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(Configure::getInstance()->max_round);
	response.WriteInt(table->m_nAnte);
	response.WriteInt(table->m_nTax);
	if(table->player_array[table->m_nDealerIndex])
		response.WriteInt(table->player_array[table->m_nDealerIndex]->id);
	else
		response.WriteInt(0);
	if(player->m_bCardStatus == CARD_KNOWN)
	{
		response.WriteByte(player->card_array[0]);
		response.WriteByte(player->card_array[1]);
		response.WriteByte(player->card_array[2]);
	}
	else
	{
		response.WriteByte(0);
		response.WriteByte(0);
		response.WriteByte(0);
	}
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt64(table->m_lCurrBetMax);
	response.WriteShort(player->optype);
	response.WriteInt64(table->m_lSumPool);
	response.WriteInt(betplayer ? betplayer->id : 0);
	response.WriteByte(remaintime>16 ? remaintime-3 : remaintime);
	response.WriteByte(Configure::getInstance()->betcointime - 3);
	if (player->m_bCardStatus == CARD_UNKNOWN)
	{
		response.WriteInt(table->m_nRase1);
		response.WriteInt(table->m_nRase2);
		response.WriteInt(table->m_nRase3);
		response.WriteInt(table->m_nRase4);
		response.WriteInt(table->m_nMaxLimit);
	}
	else
	{
		response.WriteInt(table->m_nRase2);
		response.WriteInt(table->m_nRase3);
		response.WriteInt(table->m_nRase4);
		response.WriteInt(table->m_nMaxLimit);
		response.WriteInt(table->m_nMaxLimit * 2);
	}
	response.WriteInt64(player->m_AllinCoin);
	response.WriteShort(table->m_vecBetCoin.size());
	for(size_t i = 0; i < table->m_vecBetCoin.size(); ++i)
		response.WriteInt(table->m_vecBetCoin[i]);
	response.WriteByte(table->m_nCountPlayer);
	for(size_t i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* getPlayer = table->player_array[i];
		if(getPlayer)
		{
			response.WriteInt(getPlayer->id);
			response.WriteString(getPlayer->name);
			response.WriteShort(getPlayer->m_nStatus);
			response.WriteByte(getPlayer->m_nTabIndex);
			response.WriteByte(getPlayer->m_bCardStatus);
			response.WriteInt64(getPlayer->m_lMoney);
			//说明当前正在发牌倒计时中
			response.WriteInt64(getPlayer->m_lSumBetCoin);
			response.WriteInt(getPlayer->m_nWin);
			response.WriteInt(getPlayer->m_nLose);
			response.WriteString(getPlayer->json);
			response.WriteByte(getPlayer->m_nUseMulNum);
			response.WriteByte(getPlayer->m_nUseForbidNum);
			response.WriteByte(getPlayer->m_nChangeNum);
			_LOG_DEBUG_("[Data Response] getPlayerid=[%d]\n", getPlayer->id);
			_LOG_DEBUG_("[Data Response] getPlayer->m_bCardStatus=[%d]\n", getPlayer->m_bCardStatus);

		}
	}
	response.WriteShort( 0);
    response.WriteString("");
	response.WriteByte(table->m_nMulCount);
	response.WriteByte(table->m_nMulNum1);
	response.WriteInt(table->m_nMulCoin1);
	response.WriteByte(table->m_nMulNum2);
	response.WriteInt(table->m_nMulCoin2);

	response.WriteByte(table->m_nForbidCount);
	response.WriteByte(table->m_nForbidNum1);
	response.WriteInt(table->m_nForbidCoin1);
	response.WriteByte(table->m_nForbidNum2);
	response.WriteInt(table->m_nForbidCoin2);

	response.WriteByte(table->m_nChangeCount);
	response.WriteInt(table->m_nChangeCoin1);
	response.WriteInt(table->m_nChangeCoin2);
	response.WriteInt(table->m_nChangeCoin3);
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetTableDetailProc] Send To Uid[%d] Error!\n", player->id);

	return 0;
}

int GetTableDetailProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}



