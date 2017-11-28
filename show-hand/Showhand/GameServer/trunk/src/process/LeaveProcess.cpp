#include <string>
#include "LeaveProcess.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "UdpManager.h"
#include "LogServerConnect.h"

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

	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[LeaveProcess] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2, ErrorMsg::getInstance()->getErrMsg(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[LeaveProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1, ErrorMsg::getInstance()->getErrMsg(-1), seq);
	}

	bool isturnthisuid = false;
	Player* nextplayer = NULL;
	//当用户正在玩牌的时候离开，则把他下注的金币充公
	if(player->status == STATUS_PLAYER_ACTIVE)
	{
		int64_t maxOtherBetCoin = 0;
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(table->player_array[i] && table->player_array[i]->status == STATUS_PLAYER_ACTIVE && table->player_array[i]->hascard)
			{
				if(table->player_array[i]->id != player->id)
				{
					if(table->player_array[i]->carrycoin > maxOtherBetCoin)
						maxOtherBetCoin = table->player_array[i]->carrycoin;
				}
			}
		}
		int64_t leaverSubCoin = 0;
		if(maxOtherBetCoin < player->betCoinList[0])
			leaverSubCoin = maxOtherBetCoin;
		else
			leaverSubCoin = player->betCoinList[0];

		//把离开的用户的信息记录下来，方便这局结算的时候告诉其它用户
		table->commonCoin += leaverSubCoin;
		table->leaverarry[table->leavercount].uid = uid;
		table->leaverarry[table->leavercount].tab_index = player->tab_index;
		strcpy(table->leaverarry[table->leavercount].name, player->name);
		table->leaverarry[table->leavercount].betcoin = -leaverSubCoin;
		table->leaverarry[table->leavercount].source = player->source;
		table->leaverarry[table->leavercount].pid = player->pid;
		table->leaverarry[table->leavercount].sid = player->sid;
		table->leaverarry[table->leavercount].cid = player->cid;
		table->leaverarry[table->leavercount].money = player->money - leaverSubCoin;
		table->leaverarry[table->leavercount].nwin = player->nwin;
		table->leaverarry[table->leavercount].nlose = player->nlose;
		table->leaverarry[table->leavercount].nrunaway = player->nrunaway;
		table->leaverarry[table->leavercount].m_nMagicNum = player->m_nMagicNum;
		table->leaverarry[table->leavercount].m_nMagicCoinCount = player->m_nMagicCoinCount;

		IProcess::updateDB_UserCoin(&table->leaverarry[table->leavercount], table, 0,1,1);
		table->leavercount++;
		_LOG_INFO_("[Leave] Uid=[%d] GameID=[%s] BetCountMoney=[%ld] currRound=[%d]\n", player->id, table->getGameID(), player->betCoinList[0], table->currRound);
		
		if(table->betUid == uid)
		{
			isturnthisuid = true;
			nextplayer = table->getNextBetPlayer(player,OP_THROW);
			if(nextplayer)
			{
				table->stopBetCoinTimer();
				table->setPlayerlimitcoin(nextplayer);
				table->startBetCoinTimer(nextplayer->id,Configure::getInstance()->betcointime);
				nextplayer->setBetCoinTime(time(NULL));
			}
		}
	}

	else
	{
		if(player->m_nMagicNum != 0 && player->m_nMagicCoinCount != 0)
		{
			player->money -= player->m_nMagicCoinCount;
			IProcess::updateDB_UserCoin(player, table);
		}
	}

	player->status = STATUS_PLAYER_LOGOUT;
	if(table->iscanGameOver())
		nextplayer = NULL;

	sendPlayersLeaveInfo(table, player,nextplayer,isturnthisuid, seq);
	table->playerLeave(player);
	if(isturnthisuid && !table->iscanGameOver())
	{
		if(nextplayer == NULL)
			table->setNextRound();	
	}

	Player* winner = NULL;
	//是否正在玩牌状态的人只有一个人了，则默认是这个人赢
	if(table->iscanGameOver(&winner))
		return IProcess::GameOver(table, winner, true);

	if(table->isAllReady() && table->countPlayer > 1)
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
	int sendnum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->countPlayer)
			break;
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_LEAVE,table->player_array[i]->id);
			if(table->player_array[i]->id == leavePlayer->id)
				response.SetSeqNum(seq);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->status);
			response.WriteInt(tid);
			response.WriteShort(table->status);
			response.WriteInt(leavePlayer->id);
			int64_t nextlimitcoin = -1;
			if(nextplayer)
			{
				nextlimitcoin = nextplayer->nextlimitcoin;
				response.WriteInt(nextplayer->id);
				response.WriteShort(nextplayer->optype);
				response.WriteInt64(nextlimitcoin);
				response.WriteInt64(table->currMaxCoin - nextplayer->betCoinList[table->currRound]);
			}
			else
			{
				response.WriteInt(isturnthisuid ? 0 : -1);
				response.WriteShort(0);
				response.WriteInt64(-1);
				response.WriteInt64(-1);
			}
			response.End();
			_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
			_LOG_DEBUG_("[Data Response] uid=[%d]\n",table->player_array[i]->id);
			_LOG_DEBUG_("[Data Response] status[%d]\n",table->player_array[i]->status);
			HallManager::getInstance()->sendToHall(table->player_array[i]->hallid, &response, false);
			sendnum++;
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] leavePlayer=[%d]\n", leavePlayer->id);

	return 0;
}
