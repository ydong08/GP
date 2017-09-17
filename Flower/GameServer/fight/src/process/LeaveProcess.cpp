#include <string>
#include "LeaveProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "IProcess.h"

REGISTER_PROCESS(CLIENT_MSG_LEAVE, LeaveProcess)

LeaveProcess::LeaveProcess()
{
	this->name = "LeaveProcess" ;
}

LeaveProcess::~LeaveProcess()
{
}

int LeaveProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	int   cmd   = pPacket->GetCmdType();
	short seq   = pPacket->GetSeqNum();
	int   uid   = pPacket->GetUid();
	int   uid_b = pPacket->ReadInt();
	int   tid   = pPacket->ReadInt();
	short svid  = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	if (uid != uid_b) 
	{
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(hallhandler, cmd, uid, 4, _EMSG_(4),seq);
	}
	
	_LOG_INFO_("==>[LeaveProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	if(table->m_bIsOverTimer)
	{
		_LOG_DEBUG_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		player->isdropline = true;
		return sendErrorMsg(hallhandler, cmd, uid, -35, _EMSG_(-35), seq);
	}

	bool isturnthisuid = false;
	Player* nextplayer = NULL;
	//当用户正在玩牌的时候离开，则把他下注的金币充公
	if(player->isActive())
	{
		int64_t leaverSubCoin = player->m_lSumBetCoin;
		table->m_lLeaveSumCoin += leaverSubCoin;
		//把离开的用户的信息记录下来，方便这局结算的时候告诉其它用户
		table->leaverarry[table->leavercount].uid = uid;
		table->leaverarry[table->leavercount].m_nTabIndex = player->m_nTabIndex;
		strcpy(table->leaverarry[table->leavercount].name, player->name);
		table->leaverarry[table->leavercount].m_lSumBetCoin = leaverSubCoin;
		table->leaverarry[table->leavercount].source = player->source;
		table->leaverarry[table->leavercount].pid = player->pid;
		table->leaverarry[table->leavercount].sid = player->sid;
		table->leaverarry[table->leavercount].cid = player->cid;
		table->leaverarry[table->leavercount].m_lMoney = player->m_lMoney;
		table->leaverarry[table->leavercount].m_nWin = player->m_nWin;
		table->leaverarry[table->leavercount].m_nLose = player->m_nLose;
		table->leaverarry[table->leavercount].m_nRunAway = player->m_nRunAway;
		table->leaverarry[table->leavercount].m_nUseMulCoin = player->m_nUseMulCoin;
		table->leaverarry[table->leavercount].m_nUseForbidCoin = player->m_nUseForbidCoin;
		table->leaverarry[table->leavercount].m_nUseChangeCoin = player->m_nUseChangeCoin;
		table->leaverarry[table->leavercount].m_lMaxWinMoney = player->m_lMaxWinMoney;
		table->leaverarry[table->leavercount].m_lMaxCardValue = player->m_lMaxCardValue;

		IProcess::updateDB_UserCoin(&table->leaverarry[table->leavercount], table);
		table->leavercount++;
		_LOG_INFO_("[LeaveProcess] Uid=[%d] GameID=[%s] currRound=[%d]\n", player->id, table->getGameID(), table->m_bCurrRound);

        player->recordActiveLeave(table->m_bCurrRound);

		if(table->m_nCurrBetUid == uid)
		{
			isturnthisuid = true;
			nextplayer = table->getNextBetPlayer(player,OP_THROW);
		}
		player->m_bCardStatus = CARD_DISCARD;
		if(player->m_nTabIndex == table->m_nFirstIndex)
		{
			table->setNextFirstPlayer(player);
		}
	}
	if(table->iscanGameOver())
	{
		isturnthisuid = true;
		nextplayer = NULL;
	}
	player->m_nStatus = STATUS_PLAYER_LOGOUT;
	sendPlayersLeaveInfo(table, player,nextplayer,isturnthisuid, seq);
	table->playerLeave(player);

	if(table->iscanGameOver())
		return table->gameOver();

	if(table->isAllReady() && table->m_nCountPlayer > 1)
	{
		return IProcess::GameStart(table);
	}

	return 0;
}

int LeaveProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);

	return 0;
}

int LeaveProcess::sendPlayersLeaveInfo(Table* table, Player* leavePlayer, Player* nextplayer, bool isturnthisuid, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	
	_LOG_DEBUG_("<==[LeaveProcess] Push [0x%04x]\n", CLIENT_MSG_LEAVE);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");

	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_LEAVE,table->player_array[i]->id);
			if(table->player_array[i]->id == leavePlayer->id)
				response.SetSeqNum(seq);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteByte(table->m_bCurrRound);
			response.WriteInt(leavePlayer->id);
			response.WriteInt64(table->m_lCurrBetMax);
			if(nextplayer)
			{
				response.WriteInt(nextplayer->id);
			}
			else
			{
				response.WriteInt(isturnthisuid ? 0 : -1);
			}
			response.WriteInt64(table->player_array[i]->m_lSumBetCoin);
			response.WriteInt64(table->player_array[i]->m_lMoney);
			response.WriteShort(table->player_array[i]->optype);
			response.WriteInt64(table->m_lSumPool);
			response.WriteInt64(table->player_array[i]->m_AllinCoin);
			response.End();
			_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
			_LOG_DEBUG_("[Data Response] uid=[%d]\n",table->player_array[i]->id);
			_LOG_DEBUG_("[Data Response] status[%d]\n",table->player_array[i]->m_nStatus);
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
		}
	}
	
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] leavePlayer=[%d]\n", leavePlayer->id);
	_LOG_DEBUG_("[Data Response] isturnthisuid=[%s]\n", isturnthisuid ? "true" : "false");

	return 0;
}
