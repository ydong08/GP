#include "BetCoinProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "IProcess.h"
#include "Protocol.h"
#include "ProcessManager.h"
#include "ErrorMsg.h"

BetCoinProc::BetCoinProc()
{
	this->name = "BetCoinProc";
}

BetCoinProc::~BetCoinProc()
{

} 

int BetCoinProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType() ;
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short sid = pPacket->ReadShort();
	short bettype = pPacket->ReadShort();
	int64_t betmoney = pPacket->ReadInt64();
	int x = pPacket->ReadInt();
	int y = pPacket->ReadInt();
	short realTid = tid & 0x0000FFFF;

	LOGGER(E_LOG_INFO) << "bet_args, cmd = " << TOHEX(cmd)
		<< " seq = " << seq
		<< " uid = " << uid
		<< " tid = " << tid
		<< " sid = " << sid
		<< " bettype = " << bettype
		<< " betmoney = " << betmoney
		<< " x = " << x
		<< " y = " << y
		<< " realTid = " << realTid;

    if (betmoney <= 0)
    {
        ULOGGER(E_LOG_ERROR , uid) << "bet money: " << betmoney << " is error ";
        return -1;
    }

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Table *table = Room::getInstance()->getTable(realTid);
	if(table==NULL)
	{
		_LOG_ERROR_("BetCoinProc:table is NULL\n");
		sendErrorMsg(clientHandler, cmd, uid, -2, ERRMSG(-2), seq);
		return 0;
	}

	if(! table->isBet())
	{
		_LOG_ERROR_("BetCoinProc:table is net bet m_nStatus table m_nStatus[%d]\n", table->m_nStatus);
		sendErrorMsg(clientHandler, cmd, uid, -6, ERRMSG(-6), seq);
		return 0;
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("BetCoinProc: uid[%d] Player is NULL\n", uid);
		sendErrorMsg(clientHandler, cmd, uid, -1, ERRMSG(-1), seq);
		return 0;
	}

// 	if(player->id != uid)
// 	{
// 		_LOG_ERROR_("Your uid[%d] Not Set In this index sid[%d]\n", uid, sid);
// 		sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
// 		return 0;
// 	}

	Player* banker = NULL;
	banker = table->getBanker();
	
	if(banker == NULL)
	{
		sendErrorMsg(clientHandler, cmd, uid, -27,ErrorMsg::getInstance()->getErrMsg(-27),seq);
		return 0;
	}

	if(player->id == table->bankeruid)
	{
		_LOG_ERROR_("You[%d] is the Banker, not allow to betcoin\n", uid);
		sendErrorMsg(clientHandler, cmd, uid, -7,ErrorMsg::getInstance()->getErrMsg(-7),seq);
		return 0;
	}

	//_LOG_DEBUG_("player:%d m_lBetArra:%d money:%d m_nLimitCoin:%d\n", 
	//	player->id, player->m_lBetArray[0], player->m_lMoney, table->m_nLimitCoin);
	if(player->m_lBetArray[0] == 0 && player->m_lMoney < table->m_nLimitCoin)
	{
		char				acErrorMsg[64];

		_LOG_ERROR_("Your uid[%d] m_lMoney[%ld] Not enough first bet money m_nLimitCoin[%ld]\n", uid, player->m_lMoney, table->m_nLimitCoin);
		snprintf(acErrorMsg, 64, ErrorMsg::getInstance()->getErrMsg(-13), table->m_nLimitCoin/100.0f);
		sendErrorMsg(clientHandler, cmd, uid, -13, acErrorMsg, seq);
		return 0;
	}

	if (player->m_lBetArray[0] + betmoney > table->m_nPersonLimitCoin)
	{
		char				acErrorMsg[64];

		_LOG_ERROR_("Your uid[%d] betmoney[%ld] out of Person bet limit, limit =[%ld]\n",
			uid, player->m_lBetArray[0] + betmoney, table->m_nPersonLimitCoin);
		snprintf(acErrorMsg, 64, ErrorMsg::getInstance()->getErrMsg(-32), table->m_nPersonLimitCoin / 100.0f);
		sendErrorMsg(clientHandler, cmd, uid, -32, acErrorMsg, seq);
		return 0;
	}

	if ( !IProcess::CanPlay(player) )
	{
		ULOGGER(E_LOG_ERROR, uid) << "Can't bet when now!";
		return sendErrorMsg(clientHandler, cmd, uid, -33, ERRMSG(-33), seq);
	}

	/*int64_t diffmoney = player->m_lMoney - betmoney;

	if(diffmoney < table->m_nRetainCoin)
	{
		_LOG_ERROR_("Your uid[%d] m_lMoney[%ld] Not enough betmoney[%ld] m_nRetainCoin[%d]\n", uid, player->m_lMoney, betmoney, table->m_nRetainCoin);
		sendErrorMsg(hallhandler, cmd, uid, -15,ErrorMsg::getInstance()->getErrMsg(-15),seq);
		return 0;
	}*/

	player->setActiveTime(time(NULL));
	int ret = table->playerBetCoin(player, bettype, betmoney);
	if ( ret < 0)
	{
		char				acErrorMsg[64];

		snprintf(acErrorMsg, 64, ErrorMsg::getInstance()->getErrMsg(ret), table->m_nLimitCoin / 100.0f);
		sendErrorMsg(clientHandler, cmd, uid, ret, acErrorMsg, seq);
		return 0;
	}
	
	//_LOG_INFO_("[BetCoinProc] uid=[%d] GameID=[%s] bettype=[%d] betcoin=[%d] newmoney=[%ld]\n", player->id, table->getGameID(), bettype, betmoney, player->m_lMoney);
	BetInfo betinfo;
	betinfo.nUid = player->id;
	betinfo.nSid = sid;
	betinfo.bType = bettype;
	betinfo.nBetCoin = betmoney;
	table->betVector.push_back(betinfo);

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player *receiver = table->player_array[i];
		if (receiver == NULL)
		{
			continue;
		}

		sendTabePlayersInfo(receiver, table, player, bettype, betmoney, x, y, seq);
	}

	return 0;
}


int BetCoinProc::sendTabePlayersInfo(Player* player, Table* table, Player* beter, short bettype, int64_t betmoney, int x, int y, short seq)
{
	LOGGER(E_LOG_INFO) << "broadcast bet result. cmd = " << TOHEX(CLIENT_MSG_BET_COIN)
		<< " receiver_id = " << player->id
		<< " better_id = " << beter->id
		<< " table_id = " << table->id
		<< " betmoney = " << betmoney
		<< " x = " << x
		<< " y = " << y;

	OutputPacket response;
	response.Begin(CLIENT_MSG_BET_COIN, player->id);
	{
		if (player->id == beter->id)
			response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(player->id);
		response.WriteShort(player->m_nStatus);
		response.WriteInt(table->id);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(beter->id);
		response.WriteShort(bettype);
		response.WriteInt64(betmoney);
		for (int i = 1; i < BETNUM; ++i)
		{
			response.WriteInt64(player->m_lBetArray[i]);
		}
		response.WriteInt64(beter->m_lMoney);
		for (int i = 1; i < BETNUM; ++i)
		{
			response.WriteInt64(table->m_lTabBetArray[i]);
		}
		response.WriteInt(x);
		response.WriteInt(y);
		for (int i = 1; i < BETNUM; ++i)
		{
			response.WriteInt64(table->betAreaLimit[i]);
		}
	}
	response.End();

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[BetCoinProc] Send To Uid[%d] Error!\n", player->id);
	else
		_LOG_DEBUG_("[BetCoinProc] Send To Uid[%d] Success\n", player->id);
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_BET_COIN, BetCoinProc)
