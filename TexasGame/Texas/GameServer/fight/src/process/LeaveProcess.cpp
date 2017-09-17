#include <string>
#include "LeaveProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "BaseClientHandler.h"
#include "json/json.h"
#include "IProcess.h"

using namespace std;


LeaveProcess::LeaveProcess()
{
	this->name = "LeaveProcess" ;
}

LeaveProcess::~LeaveProcess()
{
}

int LeaveProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[LeaveProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	if(table->m_bIsOverTimer)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		player->isdropline = true;
		return sendErrorMsg(hallhandler, cmd, uid, -11,ErrorMsg::getInstance()->getErrMsg(2),seq);
	}

	bool isturnthisuid = false;
	Player* nextplayer = NULL;
	//当用户正在玩牌的时候离开，则把他下注的金币充公
	if(player->isActive())
	{
		/*
		int64_t maxOtherBetCoin = 0;
		for(int i = 0; i < table->m_bMaxNum; ++i)
		{
			if(table->player_array[i] && table->player_array[i]->isActive())
			{
				if(table->player_array[i]->id != player->id)
				{
					if(table->player_array[i]->betCoinList[0] > maxOtherBetCoin)
						maxOtherBetCoin = table->player_array[i]->betCoinList[0];
				}
			}
		}
		int64_t leaverSubCoin = 0;
		if(maxOtherBetCoin < player->betCoinList[0])
			leaverSubCoin = maxOtherBetCoin;
		else
			leaverSubCoin = player->betCoinList[0];*/

		int64_t leaverSubCoin = player->betCoinList[0];
		//把离开的用户的信息记录下来，方便这局结算的时候告诉其它用户
		table->leaverarry[table->leavercount].uid = uid;
		table->leaverarry[table->leavercount].m_nTabIndex = player->m_nTabIndex;
		strcpy(table->leaverarry[table->leavercount].name, player->name);
		table->leaverarry[table->leavercount].betcoin = leaverSubCoin;
		table->leaverarry[table->leavercount].source = player->source;
		memcpy(table->leaverarry[table->leavercount].betCoinList, player->betCoinList, sizeof(int64_t)*5);
		table->leaverarry[table->leavercount].pid = player->pid;
		table->leaverarry[table->leavercount].sid = player->sid;
		table->leaverarry[table->leavercount].cid = player->cid;
		table->leaverarry[table->leavercount].m_bAllin = player->m_bAllin;
		table->leaverarry[table->leavercount].m_lMoney = player->m_lMoney - leaverSubCoin;
		table->leaverarry[table->leavercount].m_nWin = player->m_nWin;
		table->leaverarry[table->leavercount].m_nLose = player->m_nLose;
		table->leaverarry[table->leavercount].m_nRunAway = player->m_nRunAway;
		table->leaverarry[table->leavercount].m_lMaxWinMoney = player->m_lMaxWinMoney;
		table->leaverarry[table->leavercount].m_lMaxCardValue = player->m_lMaxCardValue;
		table->leaverarry[table->leavercount].m_nMagicNum = player->m_nMagicNum;
		table->leaverarry[table->leavercount].m_nMagicCoinCount = player->m_nMagicCoinCount;
		table->setSumPool();

		IProcess::updateDB_UserCoin(&table->leaverarry[table->leavercount], table);
		table->leavercount++;
		_LOG_INFO_("[LeaveProcess] Uid=[%d] GameID=[%s] BetCountMoney=[%ld] currRound=[%d]\n", player->id, table->getGameID(), player->betCoinList[0], table->m_bCurrRound);
		
		Json::Value data;
		data["BID"] = string(table->getGameID());
		data["Time"]=(int)(time(NULL) - table->getStartTime());
		data["currd"] = table->m_bCurrRound;
		data["leaveID"] = player->id;
		data["count"] = (double)player->betCoinList[0] + table->m_lTax;
		if(!table->isAllRobot())
			_LOG_REPORT_(player->id, RECORD_ACTIVE_LEAVE, "%s", data.toStyledString().c_str());
		data["errcode"] = 1;
		_UDP_REPORT_(player->id, cmd,"%s", data.toStyledString().c_str());

		if(table->m_nCurrBetUid == uid)
		{
			isturnthisuid = true;
			nextplayer = table->getNextBetPlayer(player,OP_THROW);
			if(nextplayer)
			{
				table->stopBetCoinTimer();
				table->setPlayerlimitcoin();
				table->setPlayerOptype(player,OP_THROW);
				table->startBetCoinTimer(nextplayer->id,Configure::getInstance()->betcointime);
				nextplayer->setBetCoinTime(time(NULL));
			}
		}
		player->m_bHasCard = false;
	}
	else
	{
		if(player->m_nMagicNum != 0 && player->m_nMagicCoinCount != 0)
		{
			player->m_lMoney -= player->m_nMagicCoinCount;
			IProcess::updateDB_UserCoin(player, table, player->m_nMagicCoinCount);
		}
	}

	Json::Value data;
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());

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

	if(isturnthisuid)
	{
		if(nextplayer == NULL)
			table->setNextRound();	
	}

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
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	
	_LOG_DEBUG_("<==[LeaveProcess] Push [0x%04x]\n", CLIENT_MSG_LEAVE);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");

	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *otherplayer = NULL;
		if (i < GAME_PLAYER)
			otherplayer = table->player_array[i];
		else {
			otherplayer = *it;
			it++;
		}
		if(otherplayer)
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_LEAVE,otherplayer->id);
			if(otherplayer->id == leavePlayer->id)
				response.SetSeqNum(seq);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(otherplayer->id);
			response.WriteShort(otherplayer->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(leavePlayer->id);
			response.WriteInt64(table->m_lCurrMax);
			if(nextplayer)
			{
				response.WriteInt(nextplayer->id);
			}
			else
			{
				response.WriteInt(isturnthisuid ? 0 : -1);
			}
			response.WriteInt64(otherplayer->betCoinList[table->m_bCurrRound]);
			response.WriteInt64(otherplayer->m_lCarryMoney);
			response.WriteShort(otherplayer->optype);
			response.WriteInt64(otherplayer->m_lBetLimit);
			response.WriteInt64(table->getSumPool());
			response.End();
			_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
			_LOG_DEBUG_("[Data Response] uid=[%d]\n",otherplayer->id);
			_LOG_DEBUG_("[Data Response] status[%d]\n",otherplayer->m_nStatus);
			HallManager::getInstance()->sendToHall(otherplayer->m_nHallid, &response, false);
		}
	}
	
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] leavePlayer=[%d]\n", leavePlayer->id);
	_LOG_DEBUG_("[Data Response] isturnthisuid=[%s]\n", isturnthisuid ? "true" : "false");

	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_LEAVE, LeaveProcess)