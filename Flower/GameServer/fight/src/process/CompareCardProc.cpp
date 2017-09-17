#include <string>
#include "CompareCardProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "IProcess.h"

REGISTER_PROCESS(CLIENT_MSG_COMPARE_CARD, CompareCardProc)

CompareCardProc::CompareCardProc()
{
	this->name = "CompareCardProc" ;
}

CompareCardProc::~CompareCardProc()
{
}

int CompareCardProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int   cmd   = pPacket->GetCmdType();
	short seq   = pPacket->GetSeqNum();
	int   uid   = pPacket->GetUid();
	int   uid_b = pPacket->ReadInt();
	int   tid   = pPacket->ReadInt();
	int   oppid = pPacket->ReadInt();
	short svid  = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	if (uid != uid_b) 
	{
		ULOGGER(E_LOG_ERROR, uid) << "fucking userid operator uid=" << uid_b; 
		return sendErrorMsg(clientHandler, cmd, uid, 4, _EMSG_(4),seq);
	}

	_LOG_INFO_("==>[CompareCardProc] [0x%04x] uid[%d] oppid[%d]\n", cmd, uid, oppid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, _EMSG_(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	Player* opponent = table->getPlayer(oppid);
	if(player == NULL)
	{
		_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	if(opponent == NULL)
	{
		_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] opponent:%d not in This Table\n",uid, tid, realTid, oppid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(-1), seq);
	}

	if( !player->isActive())
	{
		_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -11, _EMSG_(-11), seq);
	}

	if( !opponent->isActive())
	{
		_LOG_ERROR_("[CompareCardProc] oppid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",oppid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -11, _EMSG_(-11), seq);
	}

	if(player->m_bCardStatus == CARD_DISCARD /*|| player->m_bCardStatus == CARD_LOSE*/ || opponent->m_bCardStatus == CARD_DISCARD /*|| opponent->m_bCardStatus == CARD_LOSE*/)
	{
		_LOG_ERROR_("[CompareCardProc] oppid=[%d] or uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] not have card\n",oppid, uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -21, _EMSG_(-21), seq);
	}

	if(table->m_nCurrBetUid != uid)
	{
		_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] betUid[%d] is not this uid\n",uid, tid, realTid, table->m_nCurrBetUid);
		return sendErrorMsg(clientHandler, cmd, uid, -12, _EMSG_(-12), seq);
	}

    if (player->m_lMoney <= 0)
    {
        _LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] player->m_lMoney=[%ld] is less than zero\n" , uid , tid , realTid , player->m_lMoney );
        return sendErrorMsg(clientHandler , cmd , uid , -4 , _EMSG_(-4) , seq);
    }

	int64_t comparecoin = Player::calculateCompMoney(player, table->m_BetMaxCardStatus, table->m_lCurrBetMax, table->m_nMaxLimit);

	if(opponent->m_nUseMulNum != 0)
		comparecoin = comparecoin * opponent->m_nUseMulNum;

	comparecoin = comparecoin > player->m_lMoney ? player->m_lMoney : comparecoin;

	//当比牌花费小于自身金币则禁比生效，否则不生效
	if(comparecoin < player->m_lMoney)
	{
		if(player->m_nUseForbidNum != 0 || opponent->m_nUseForbidNum != 0)
		{
			_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] forbidnum[%d] or opponet[%d] forbidnum[%d] not zero\n",
				uid, tid, realTid, player->m_nUseForbidNum, opponent->id, opponent->m_nUseForbidNum);
			return sendErrorMsg(clientHandler, cmd, uid, -27, _EMSG_(-27), seq);
		}
	}

	if(player->m_lMoney < comparecoin)
	{
		_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] player->m_lMoney=[%ld] is less bet comparecoin[%d]\n",uid, tid, realTid, player->m_lMoney, comparecoin);
		return sendErrorMsg(clientHandler, cmd, uid, -4, _EMSG_(-4), seq);
	}

	if(table->m_bIsOverTimer)
	{
		_LOG_ERROR_("[CompareCardProc] table is IsOverTimer uid=[%d] tid=[%d] realTid=[%d] Table Status=[%d]\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -1, _EMSG_(2), seq);
	}

	short compareRet = table->compareCard(player, opponent);
	if(compareRet < 0)
	{
		_LOG_ERROR_("[CompareCardProc] uid=[%d] tid=[%d] realTid=[%d] player->m_lMoney=[%ld] Compare Card Error!\n",uid, tid, realTid, player->m_lMoney);
		return sendErrorMsg(clientHandler, cmd, uid, -22, _EMSG_(-22), seq);
	}

	table->playerBetCoin(player, comparecoin);
	Player* nextplayer = NULL;

	player->m_bCompare = true;
	if(compareRet == 1)
	{
		//opponent->m_bCardStatus = CARD_LOSE;
		opponent->m_bCardStatus = CARD_DISCARD;
		if(opponent->m_nTabIndex == table->m_nFirstIndex)
		{
			table->setNextFirstPlayer(opponent);
		}
		nextplayer = table->getNextBetPlayer(player, 0);
		if(nextplayer)
		{		
			table->stopBetCoinTimer();
			table->startBetCoinTimer(nextplayer->id,Configure::getInstance()->betcointime + 4);
			nextplayer->setBetCoinTime(time(NULL));
		}

        int activeCount = table->getActivePlayingPlayerCount();

        _LOG_ERROR_("[check did need to start force campare card] uid=[%d] activeCount=[%d] player->m_lMoney=[%ld] !" , uid , activeCount , player->m_lMoney);
        if (2 >= activeCount && player->m_lMoney == 0)
        {
            IProcess::ForceCampareCard(table , player , nextplayer , seq, true);
        }
	}

	if(compareRet == 0 || compareRet == DRAW)
	{
		//player->m_bCardStatus = CARD_LOSE;
		player->m_bCardStatus = CARD_DISCARD;
		nextplayer = table->getNextBetPlayer(player, OP_THROW);
		if(player->m_nTabIndex == table->m_nFirstIndex)
		{
			table->setNextFirstPlayer(player);
		}
		compareRet = 0;
		if(nextplayer)
		{
			table->stopBetCoinTimer();
			table->startBetCoinTimer(nextplayer->id,Configure::getInstance()->betcointime + 4);
			nextplayer->setBetCoinTime(time(NULL));
		}
	}
	//说明此用户已经allin了
	/*if(player->m_lMoney <= 0)
	{
		player->m_bAllin = true;
	}*/
	
	_LOG_INFO_("[CompareCardProc] Uid=[%d] GameID=[%s] ComCardCoin=[%ld] m_lSumBetCoin=[%ld] currRound=[%d]\n", player->id, table->getGameID(), comparecoin, player->m_lSumBetCoin, table->m_bCurrRound);
	
    player->recordCompareCard(table->m_bCurrRound , oppid , comparecoin, compareRet);
	//设置下一个应该下注的用户
	player->setActiveTime(time(NULL));
	
	if(table->iscanGameOver())
		nextplayer = NULL;

	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			IProcess::sendCompareInfoToPlayers(table->player_array[i], table, player, opponent, compareRet, nextplayer, comparecoin, seq);
		}
	}
	if(table->iscanGameOver())
		return table->gameOver(true);
	return 0;
}



int CompareCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

