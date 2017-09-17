#include "BetCoinProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "IProcess.h"
#include "Protocol.h"
#include "ProcessManager.h"

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
    int uid = pPacket->GetUid();
	int uid_b = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short sid = pPacket->ReadShort();
	short bettype = pPacket->ReadShort();
	int64_t betmoney = pPacket->ReadInt64();
	int x = pPacket->ReadInt();
	int y = pPacket->ReadInt();

    if (uid != uid_b)
    {
        ULOGGER(E_LOG_ERROR , uid) << "uid in packet is " << uid_b;
    }

    if (betmoney <= 0)
    {
        ULOGGER(E_LOG_ERROR , uid) << "bet money: " << betmoney << " is error ";
        return -1;
    }

	_LOG_DEBUG_("==>[BetCoinProc]  cmd[0x%04x] uid[%d] sid=[%d]\n", cmd, uid, sid);
	_LOG_DEBUG_("[DATA Parse] bettype=[%d] betmoney=[%lu] \n", bettype, betmoney);
	CDLSocketHandler* hallhandler = clientHandler;

	Room* room = Room::getInstance();
    Table *table = room->getTable();
	//int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("BetCoinProc:table is NULL\n");
		sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
		return 0;
	}

	if(! table->isBet())
	{
		_LOG_ERROR_("BetCoinProc:table is net bet m_nStatus table m_nStatus[%d]\n", table->m_nStatus);
		sendErrorMsg(hallhandler, cmd, uid, -6,ErrorMsg::getInstance()->getErrMsg(-6),seq);
		return 0;
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("BetCoinProc: uid[%d] Player is NULL\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		return 0;
	}

	Player* banker = NULL;
	if (table->bankersid >= 0)
		banker = table->player_array[table->bankersid];


	if (banker)
	{
		if ((table->m_lTabBetArray[0] + betmoney) * 10 > banker->m_lMoney)
		{
			_LOG_ERROR_("Your uid[%d] m_lMoney[%ld] Not enough betmoney[%ld] playerbet[%ld]\n", uid, banker->m_lMoney, betmoney, table->m_lTabBetArray[0] + betmoney);
			sendErrorMsg(hallhandler, cmd, uid, -14, ErrorMsg::getInstance()->getErrMsg(-14), seq);
			return 0;
		}
	}
	else
	{
		_LOG_ERROR_("Your uid[%d] Not Set In this index sid[%d]\n", uid, sid);
		sendErrorMsg(hallhandler, cmd, uid, -27, ErrorMsg::getInstance()->getErrMsg(-27), seq);
		return 0;
	}

	int64_t total_money = table->m_lTabBetArray[bettype] + betmoney;
	if (total_money > table->m_lTabBetLimit)
	{
		ULOGGER(E_LOG_ERROR, uid) << "out of table area bet limit, limit = " << table->m_lTabBetLimit;
		sendErrorMsg(hallhandler, cmd, uid, -31, ErrorMsg::getInstance()->getErrMsg(-31), seq);
		return 0;
	}

// 	if(player->id != uid)
// 	{
// 		_LOG_ERROR_("Your uid[%d] Not Set In this index sid[%d]\n", uid, sid);
// 		sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
// 		return 0;
// 	}

	//_LOG_DEBUG_("player:%d m_lBetArra:%d money:%d m_nLimitCoin:%d\n", 
	//	player->id, player->m_lBetArray[0], player->m_lMoney, table->m_nLimitCoin);
	if(player->m_lBetArray[0] == 0 && player->m_lMoney < table->m_nLimitCoin)
	{
		char				acErrorMsg[64];

		_LOG_ERROR_("Your uid[%d] m_lMoney[%ld] Not enough first bet money m_nLimitCoin[%ld]\n", uid, player->m_lMoney, table->m_nLimitCoin);
		snprintf(acErrorMsg, 64, ErrorMsg::getInstance()->getErrMsg(-13), table->m_nLimitCoin / 100.0f);
		sendErrorMsg(hallhandler, cmd, uid, -13, acErrorMsg, seq);
		return 0;
	}

	int64_t diffmoney = (player->m_lBetArray[0]+player->m_lMoney) - (player->m_lBetArray[0]+betmoney) * 10;

	if (diffmoney < table->m_nRetainCoin)
	{
		_LOG_ERROR_("Your uid[%d] m_lMoney[%ld] Not enough betmoney[%ld] m_nRetainCoin[%d]\n", uid, player->m_lMoney, betmoney, table->m_nRetainCoin);
		sendErrorMsg(hallhandler, cmd, uid, -15,ErrorMsg::getInstance()->getErrMsg(-15),seq);
		return 0;
	}

	if(player->id == table->bankeruid)
	{
		_LOG_ERROR_("You[%d] is the Banker, not allow to betcoin\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -7,ErrorMsg::getInstance()->getErrMsg(-7),seq);
		return 0;
	}
	
	player->setActiveTime(time(NULL));
	if (table->playerBetCoin(player, bettype, betmoney) < 0)
	{
		sendErrorMsg(hallhandler, cmd, uid, -8,ErrorMsg::getInstance()->getErrMsg(-8),seq);
		return 0;
	}
	
	_LOG_INFO_("[BetCoinProc] uid=[%d] GameID=[%s] bettype=[%d] betcoin=[%d] newmoney=[%ld]\n", player->id, table->getGameID(), bettype, betmoney, player->m_lMoney);
	BetInfo betinfo;
	betinfo.nUid = player->id;
	betinfo.nSid = sid;
	betinfo.bType = bettype;
	betinfo.nBetCoin = betmoney;
	table->betVector.push_back(betinfo);

	sendTabePlayersInfo(player, table, player, bettype, betmoney, x, y, seq);
	if (player->source != E_MSG_SOURCE_ROBOT)
	{
		IProcess::NotifyRobot(table, banker, player, bettype, betmoney);
	}
	return 0;
}


int BetCoinProc::sendTabePlayersInfo(Player* player, Table* table, Player* beter, short bettype, int64_t betmoney,int x, int y, short seq)
{
	OutputPacket response;
	response.Begin(CLIENT_MSG_BET_COIN, player->id);
	if(player->id == beter->id)
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
	int i = 0;
	for(i = 1; i < 5; ++i)
		response.WriteInt64(player->m_lBetArray[i]);
	response.WriteInt64(beter->m_lMoney);
	for(i = 1; i < 5; ++i)
		response.WriteInt64(table->m_lTabBetArray[i]);
	response.WriteInt(x);
	response.WriteInt(y);
	if (player->source == E_MSG_SOURCE_ROBOT)
	{
		for (i = 1; i < BETNUM; ++i)
		{
			response.WriteInt64(table->m_lRealBetArray[i]);
		}
	}
	response.End();
	
	/*_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] bettype=[%d]\n", bettype);
	_LOG_DEBUG_("[Data Response] betmoney=[%d]\n", betmoney);
	_LOG_DEBUG_("[Data Response] betid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] betplayer=[%d]\n", player->betplayer);
	_LOG_DEBUG_("[Data Response] betbanker=[%d]\n", player->betbanker);
	_LOG_DEBUG_("[Data Response] bettie=[%d]\n", player->bettie);
	_LOG_DEBUG_("[Data Response] betKplayer=[%d]\n", player->betKplayer);
	_LOG_DEBUG_("[Data Response] betKbanker=[%d]\n", player->betKbanker);
	_LOG_DEBUG_("[Data Response] betplayerLimit=[%d]\n", table->betplayerLimit);
	_LOG_DEBUG_("[Data Response] betbankerLimit=[%d]\n", table->betbankerLimit);
	_LOG_DEBUG_("[Data Response] bettieLimit=[%d]\n", table->bettieLimit);
	_LOG_DEBUG_("[Data Response] betKplayerLimit=[%d]\n", table->betKplayerLimit);
	_LOG_DEBUG_("[Data Response] betKbankerLimit=[%d]\n", table->betKbankerLimit);
	_LOG_DEBUG_("[Data Response] push to uid=[%d] m_nStatus=[%d]\n", player->id,player->m_nStatus);*/
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[BetCoinProc] Send To Uid[%d] Error!\n", player->id);
	//else
	//	_LOG_DEBUG_("[BetCoinProc] Send To Uid[%d] Success\n", player->id);
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_BET_COIN, BetCoinProc)