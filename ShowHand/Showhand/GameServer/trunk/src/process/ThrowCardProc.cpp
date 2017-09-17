#include <string>
#include "ThrowCardProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "UdpManager.h"
#include "LogServerConnect.h"

using namespace std;


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
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[ThrowCardProc] [0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	if( ! player->isActive())
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] tstatus=[%d] is not active\n",
			uid, tid, realTid, table->status);
		return sendErrorMsg(hallhandler, cmd, uid, -11,ErrorMsg::getInstance()->getErrMsg(-11),seq);
	}

	if(table->betUid != uid)
	{
		_LOG_ERROR_("[ThrowCardProc] uid=[%d] tid=[%d] realTid=[%d] betUid[%d] is not this uid\n",
			uid, tid, realTid, table->betUid);
		return sendErrorMsg(hallhandler, cmd, uid, -12,ErrorMsg::getInstance()->getErrMsg(-12),seq);
	}

	_LOG_INFO_("[ThrowCard] Uid=[%d] GameID=[%s] BetCountMoney=[%ld] currRound=[%d]\n", player->id, table->getGameID(), player->betCoinList[0], table->currRound);
	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["Time"]=(int)(time(NULL) - table->getStartTime());
	data["currd"] = table->currRound;
	data["throwID"] = player->id;
	data["count"] = (double)player->betCoinList[0];
	if(!table->isAllRobot())
		_LOG_REPORT_(player->id, RECORD_THROW_CARD, "%s", data.toStyledString().c_str());
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());
	//表示此人已经弃牌
	player->hascard = false;

	Player* winner = NULL;
	//设置下一个应该下注的用户
	Player* nextplayer = table->getNextBetPlayer(player,OP_THROW);
	if(table->iscanGameOver(&winner))
		nextplayer = NULL;
	if(nextplayer)
	{
		table->setPlayerlimitcoin(nextplayer);
	}
	int sendNum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendNum == table->countPlayer)
			break;
		if(table->player_array[i])
		{
			sendThrowInfoToPlayers(table->player_array[i], table, player, nextplayer,seq);
			sendNum++;
		}
	}
	winner = NULL;
	if(table->iscanGameOver(&winner))
		return IProcess::GameOver(table, winner, true);

	//当前已经没有下一个用户下注了此轮结束
	if(nextplayer == NULL)
	{
		table->setNextRound();
	}
	else
	{
		table->stopBetCoinTimer();
		table->startBetCoinTimer(nextplayer->id,Configure::getInstance()->betcointime);
		nextplayer->setBetCoinTime(time(NULL));
	}
	return 0;
}


int ThrowCardProc::sendThrowInfoToPlayers(Player* player, Table* table, Player* throwcallplayer,Player* nextplayer, short seq)
{
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_THROW_CARD, player->id);
	if(player->id == throwcallplayer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteShort(table->currRound);
	response.WriteInt(throwcallplayer->id);
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
		response.WriteInt(0);
		response.WriteShort(0);
		response.WriteInt64(-1);
		response.WriteInt64(-1);
	}
	response.End();
	_LOG_DEBUG_("<==[ThrowCardProc] Push [0x%04x] to uid=[%d]\n", CLIENT_MSG_THROW_CARD, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	_LOG_DEBUG_("[Data Response] uid=[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] status=[%d]\n",player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] currRound=[%d]\n", table->currRound);
	_LOG_DEBUG_("[Data Response] throwcallplayer_id=[%d]\n",throwcallplayer->id);
	_LOG_DEBUG_("[Data Response] nextplayer=[%d]\n", nextplayer ? nextplayer->id : 0);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", nextplayer ? nextplayer->optype : 0);
	_LOG_DEBUG_("[Data Response] nextlimitcoin=[%ld]\n", nextlimitcoin);
	_LOG_DEBUG_("[Data Response] differcoin=[%ld]\n",nextplayer ? table->currMaxCoin - nextplayer->betCoinList[table->currRound] : -1);
	
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
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

