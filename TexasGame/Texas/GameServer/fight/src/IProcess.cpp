#include "IProcess.h"
#include "Configure.h"
#include "GameCmd.h"
#include "ProtocolServerId.h"
#include "ProtocolClientId.h"
#include "HallManager.h"
#include "Logger.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "Util.h"
#include "RedisOprs.h"
#include "RedisLogic.h"
#include "json/json.h"
#include "Protocol.h"
#include "GameApp.h"
#include "GameUtil.h"

int IProcess::sendToRobotCard(Player* player, Table* table)
{
	if(player==NULL || table == NULL)
		return 0;
	if(player->source != 30)
		return 0;
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	OutputPacket response;
	response.Begin(SERVER_MSG_ROBOT, player->id);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_nCountPlayer);
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			for(int j = 0; j < 5; j++)
				response.WriteByte(otherplayer->cbEndCardData[j]);
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendToRobotCard] Send To Uid[%d] Error!\n", player->id);
	return 0;
}

int IProcess::GameStart( Table* table)
{
	if(table == NULL)
		return -1;
	table->gameStart();

	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *player = NULL;
		if (i < GAME_PLAYER)
			player = table->player_array[i];
		else {
			player = *it;
			it++;
		}
		if(player)
		{
			if(player->isActive())
			{
				if(player->source == 30)
				{
					sendToRobotCard(player, table);
				}
			}
			sendGameStartInfo(player, table);
		}
	}

	table->setNextRound();

	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["svid"] = Configure::getInstance()->server_id;
	data["tid"] = table->id;
	data["level"] = table->m_nType;
	data["ante"] = (double)table->m_lBigBlind;
	data["tax"] = (double)table->m_lTax;
	char centercard[32] = {0};
	for(int j = 0; j < 5; j++)
		sprintf(centercard+j*5, j == 4 ? "0x%02x" : "0x%02x_", table->m_cbCenterCardData[j]);
	data["centercard"] = string(centercard);
	data["num"] = table->m_bMaxNum;
	for(int i = 0; i < table->m_bMaxNum; ++i)
	{
		char key[16] = {0};
		sprintf(key, "p%d", i);
		if(table->player_array[i] == NULL)
			data[key] = 0;
		else
		{
			data[key] = table->player_array[i]->id;
			sprintf(key, "money%d", i);
			data[key] = (double)table->player_array[i]->m_lMoney + table->m_lTax;
			sprintf(key, "carrycoin%d", i);
			data[key] = (double)table->player_array[i]->m_lCarryMoney;
			sprintf(key, "tabindex%d", i);
			data[key] = table->player_array[i]->m_nTabIndex;
			sprintf(key, "source%d", i);
			data[key] = table->player_array[i]->source;
			char handcard[16] = {0};
			for(int j = 0; j < 2; j++)
				sprintf(handcard+j*5, j == 1 ? "0x%02x" : "0x%02x_", table->player_array[i]->card_array[j]);
			sprintf(key, "handcard%d", i);
			data[key] = string(handcard);
			char szcard[32] = {0};
			for(int j = 0; j < 5; j++)
				sprintf(szcard+j*5, j == 4 ? "0x%02x" : "0x%02x_", table->player_array[i]->cbEndCardData[j]);
			sprintf(key, "cardArry%d", i);
			data[key] = string(szcard);
		}
	}

	if(!table->isAllRobot())
	{
		_LOG_DEBUG_("Not All Robot Send To Log Server True\n");
		_LOG_REPORT_(table->m_nRaseUid, RECORD_GAME_START, "%s", data.toStyledString().c_str());
	}

	return 0;
}


int IProcess::sendGameStartInfo(Player* player, Table* table)
{
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	int sendNum = 0;
	int i = 0;
	OutputPacket response;
	response.Begin(GMSERVER_GAME_START, player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteInt64(table->m_lBigBlind);
	response.WriteInt64(table->m_lGameLimt);
	response.WriteInt64(table->m_lTax);
	response.WriteInt64(table->getSumPool());
	response.WriteByte(table->m_nDealerIndex);
	response.WriteInt(table->m_nSmallBlindUid);
	response.WriteInt64(table->player_array[table->m_nSmallBlindIndex]->betCoinList[0]);
	response.WriteInt(table->m_nBigBlindUid);
	response.WriteInt64(table->player_array[table->m_nBigBlindIndex]->betCoinList[0]);
	response.WriteByte(Configure::getInstance()->betcointime - 3);
	response.WriteByte(table->m_nCountPlayer);
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if (sendNum == table->m_nCountPlayer)
			break; 
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteShort(otherplayer->m_nStatus);
			response.WriteByte(otherplayer->m_nTabIndex);
			response.WriteInt64(otherplayer->m_lCarryMoney);
			response.WriteInt64(otherplayer->betCoinList[0]);
			printf("uid:%d betcoin:%ld\n", otherplayer->id, otherplayer->betCoinList[0]);
			sendNum++;
		}       
	}
	response.WriteShort(player->m_pTask ? player->m_nGetRoll : 0);
    response.WriteString(player->m_pTask ? player->m_pTask->taskname : "");
	response.WriteInt(table->m_nMustBetCoin);
	response.End();

	_LOG_DEBUG_("<==[sendGameStartInfo] Push [0x%04x] to uid=[%d]\n", GMSERVER_GAME_START, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_lBigBlind=[%ld]\n", table->m_lBigBlind);
	_LOG_DEBUG_("[Data Response] m_nDealerIndex=[%d]\n", table->m_nDealerIndex);
	_LOG_DEBUG_("[Data Response] getSumPool=[%d]\n", table->getSumPool());

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendGameStartInfo] Send To Uid[%d] Error!\n", player->id);

	return 0;
}

int IProcess::sendRoundCard(Player* player, Table* table, short currround)
{
	if(player == NULL || table == NULL)
		return -1;
	//只允许当前轮数在1 - 6 之间，非别代表发牌第一轮至第四轮，5 代表大家翻底牌决定胜负
	if(currround < 0 || currround > 5)
		return -1;

	int i = 0;
	int j = 0;
	int sendNum = 0;
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;

	OutputPacket response;
	response.Begin(GMSERVER_MSG_SEND_CARD, player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(player->card_array[0]);
	response.WriteByte(player->card_array[1]);
	response.WriteInt(table->m_pFirstBetter ? table->m_pFirstBetter->id : 0);
	response.WriteShort(player->optype);
	response.WriteInt64(player->betCoinList[currround]);
	response.WriteInt64(player->betCoinList[0]);
	response.WriteInt64(player->m_lBetLimit);
	response.WriteInt64(table->m_lCurrMax);
	response.WriteByte(currround);
	if(currround == 1)
	{
		player->m_bTempCardType = table->m_GameLogic.GetTwoCardType(player->card_array, 2);
		if(player->m_bHasCard)
			response.WriteByte(player->m_bTempCardType);
		else
			response.WriteByte(0);
		_LOG_DEBUG_("uid:%d GetTwoCardType:%d\n", player->id, table->m_GameLogic.GetTwoCardType(player->card_array, 2));
	}
	if(currround == 2)
	{
		response.WriteByte(table->m_cbCenterCardData[0]);
		response.WriteByte(table->m_cbCenterCardData[1]);
		response.WriteByte(table->m_cbCenterCardData[2]);
		BYTE bTempCardDate[5] = {0};
		bTempCardDate[0] = player->card_array[0];
		bTempCardDate[1] = player->card_array[1];
		bTempCardDate[2] = table->m_cbCenterCardData[0];
		bTempCardDate[3] = table->m_cbCenterCardData[1];
		bTempCardDate[4] = table->m_cbCenterCardData[2];
		table->m_GameLogic.SortCardList(bTempCardDate, 5);
		if(player->m_bHasCard)
		{
			player->m_bTempCardType = table->m_GameLogic.GetCardType(bTempCardDate, 5);
			response.WriteByte(1);
			for(int j = 0; j < MAX_CENTERCOUNT; j++)
				response.WriteByte(bTempCardDate[j]);
			response.WriteByte(player->m_bTempCardType);
		}
		else
			response.WriteByte(0);
		_LOG_DEBUG_("uid:%d GetCardType2222222:%d\n", player->id, table->m_GameLogic.GetCardType(bTempCardDate, 5));
	}

	if(currround == 3)
	{
		response.WriteByte(table->m_cbCenterCardData[0]);
		response.WriteByte(table->m_cbCenterCardData[1]);
		response.WriteByte(table->m_cbCenterCardData[2]);
		response.WriteByte(table->m_cbCenterCardData[3]);
		if(player->m_bHasCard)
		{
			BYTE cbTempEndCardData[5];
			BYTE bValue = table->m_GameLogic.FiveFromSix(player->card_array,MAX_COUNT,table->m_cbCenterCardData,4,cbTempEndCardData,MAX_CENTERCOUNT);
			player->m_bTempCardType = bValue;
			response.WriteByte(1);
			for(int j = 0; j < MAX_CENTERCOUNT; j++)
				response.WriteByte(cbTempEndCardData[j]);
			response.WriteByte(bValue);
			_LOG_DEBUG_("uid:%d bValue:%d\n", player->id, bValue);
		}
		else
			response.WriteByte(0);
	}

	if(currround == 4)
	{
		response.WriteByte(table->m_cbCenterCardData[0]);
		response.WriteByte(table->m_cbCenterCardData[1]);
		response.WriteByte(table->m_cbCenterCardData[2]);
		response.WriteByte(table->m_cbCenterCardData[3]);
		response.WriteByte(table->m_cbCenterCardData[4]);
		if(player->m_bHasCard)
		{
			response.WriteByte(1);
			for(int j = 0; j < MAX_CENTERCOUNT; j++)
				response.WriteByte(player->cbEndCardData[j]);
			response.WriteByte(player->m_nEndCardType);
			_LOG_DEBUG_("bValu22222e:%d\n", player->m_nEndCardType);
		}
		else
			response.WriteByte(0);
	}

	if(currround == 5)
	{
		response.WriteByte(table->m_cbCenterCardData[0]);
		response.WriteByte(table->m_cbCenterCardData[1]);
		response.WriteByte(table->m_cbCenterCardData[2]);
		response.WriteByte(table->m_cbCenterCardData[3]);
		response.WriteByte(table->m_cbCenterCardData[4]);
		response.WriteByte(table->m_nCountPlayer);
		for(i = 0; i<table->m_bMaxNum; ++i)
		{
			Player* otherplayer = table->player_array[i];
			if(otherplayer)
			{
				response.WriteInt(otherplayer->id);
				response.WriteShort(otherplayer->m_nStatus);
				response.WriteInt64(otherplayer->betCoinList[0]);
				if(otherplayer->m_bHasCard)
				{
					response.WriteByte(1);
					response.WriteByte(otherplayer->card_array[0]);
					response.WriteByte(otherplayer->card_array[1]);
					for(int j = 0; j < MAX_CENTERCOUNT; j++)
						response.WriteByte(otherplayer->cbEndCardData[j]);
					response.WriteByte(otherplayer->m_nEndCardType);
				}
				else
					response.WriteByte(0);
			}
		}
	}
	response.End();

	_LOG_DEBUG_("<==[sendRoundCard] Push [0x%04x] to uid=[%d]\n", GMSERVER_MSG_SEND_CARD, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_nCountPlayer=[%d]\n", table->m_nCountPlayer);
	_LOG_DEBUG_("[Data Response] currround=[%d]\n", currround);
	_LOG_DEBUG_("[Data Response] m_pFirstBetter=[%d]\n", table->m_pFirstBetter? table->m_pFirstBetter->id : 0);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendRoundCard] Send To Uid[%d] Error!\n", player->id);

	return 0;
}

int IProcess::sendPoolinfo(Table* table)
{
	if(table == NULL)
		return -1;

	int i = 0;
	int j = 0;
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;

	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *player = NULL;
		if (i < GAME_PLAYER)
			player = table->player_array[i];
		else {
			player = *it;
			it++;
		}
		if(player)
		{
			OutputPacket response;
			response.Begin(CLIENT_GET_POOL, player->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(player->id);
			response.WriteShort(player->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteByte(table->m_bCurrRound);
			if(table->m_PoolArray[0].betCoinCount > 0)
			{
				response.WriteByte(table->m_nPoolNum+1);
				for(i = 0; i < table->m_nPoolNum+1; ++i)
					response.WriteInt64(table->m_PoolArray[i].betCoinCount);
			}
			else
				response.WriteByte(0);
			response.End();
			if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
				_LOG_ERROR_("[sendPoolinfo] Send To Uid[%d] Error!\n", player->id);
		}
	}
	

	_LOG_DEBUG_("<==[sendPoolinfo] Push [0x%04x] \n", CLIENT_GET_POOL);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_nPoolNum=[%d]\n", table->m_nPoolNum);

	return 0;
}

int IProcess::pushPrivateOprate(Table* table)
{
	if(table == NULL)
		return -1;

	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;

	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *player = NULL;
		if (i < GAME_PLAYER)
			player = table->player_array[i];
		else {
			player = *it;
			it++;
		}
		if(player)
		{
			OutputPacket response;
			response.Begin(PUSH_PRIVATE_OP, player->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(player->id);
			response.WriteShort(player->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteShort(player->optype);
			response.End();
			if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
				_LOG_ERROR_("[pushPrivateOprate] Send To Uid[%d] Error!\n", player->id);
		}
	}
	

	_LOG_DEBUG_("<==[pushPrivateOprate] Push [0x%04x] \n", PUSH_PRIVATE_OP);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);

	return 0;
}


int IProcess::GameOver( Table* table)
{
	if(table == NULL)
	{
		_LOG_ERROR_("[GameOver] table[%p]\n", table);
		return -1;
	}
	int i = 0;
	int sendNum = 0;
    std::string endtime = Util::formatGMTTimeS();

	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* player = table->player_array[i];
		if(player && player->isGameOver())
		{
			if (player->m_lFinallGetCoin > 0)
			{
				player->m_nWin++;
				player->m_nTax = int(float(player->m_lFinallGetCoin) * table->m_lTax);
				player->m_nTax = player->m_nTax > 0 ? player->m_nTax : 1;
				player->m_lFinallGetCoin -= player->m_nTax;

				if (E_MSG_SOURCE_ROBOT == player->source)
				{
					int64_t lRobotWin = player->m_lFinallGetCoin + player->m_nTax;
					if (!RedisLogic::AddRobotWin(Tax(), player->pid, Configure::getInstance()->server_id, (int)lRobotWin))
						LOGGER(E_LOG_DEBUG) << "OperationRedis::AddRobotWin Error, pid=" << player->pid << ", server_id=" << Configure::getInstance()->server_id << ", win=" << lRobotWin;
				}
				else
				{
					if (!RedisLogic::UpdateTaxRank(Tax(), player->pid, Configure::getInstance()->server_id, GAME_ID, Configure::getInstance()->level, player->tid, player->id, player->m_nTax))
						LOGGER(E_LOG_DEBUG) << "OperationRedis::GenerateTip Error, pid=" << player->pid << ", server_id=" << Configure::getInstance()->server_id << ", gameid=" << GAME_ID << ", level=" << Configure::getInstance()->level << ", id=" << player->id << ", Tax=" << player->m_nTax;
				}
			}
			else if (player->m_lFinallGetCoin < 0)
			{
				player->m_nLose++;
				player->m_nTax = 0;
			}
			
            if (!player->isdropline  && player->source != 30)
            {
                RedisLogic::setPlayerOffline(Offline(), GAME_ID, Configure::getInstance()->level,
                                            player->id, player->m_lFinallGetCoin, endtime);
            }

			player->m_lMoney += player->m_lFinallGetCoin;
			player->m_lCarryMoney += player->m_lFinallGetCoin;
			
			ULOGGER(E_LOG_INFO, player->id) << "money=" << player->m_lMoney << " carrymoney=" << player->m_lCarryMoney 
 				<< "FinallGetCoin =" << player->m_lFinallGetCoin << " tax=" << player->m_nTax;
			
			if(player->source != 30)
			{
				if(player->m_lFinallGetCoin > 0)
				{
					player->m_bCompleteTask = TaskManager::getInstance()->calcPlayerComplete(player, table);
					RedisOprs::ModUserWinCoin(player->id, player->m_lFinallGetCoin);
				}
				RedisOprs::ModUserWealth(player->id, player->m_lMoney);
			}

			short loadtype = 10;
			if(Configure::getInstance()->level <= LEVEL1)
				loadtype = 1;
			else if (Configure::getInstance()->level > LEVEL1 && Configure::getInstance()->level <= LEVEL2)
				loadtype = 2;
			else if (Configure::getInstance()->level > LEVEL2 && Configure::getInstance()->level <= LEVEL3)
				loadtype = 3;

			int64_t winmoney = player->m_lFinallGetCoin;
			std::string strTrumpt;
			if (GameUtil::getDisplayMessage(strTrumpt, GAME_ID, Configure::getInstance()->level, player->name, winmoney, Configure::getInstance()->wincoin1,
				Configure::getInstance()->wincoin2, Configure::getInstance()->wincoin3, Configure::getInstance()->wincoin4))
			{
				AllocSvrConnect::getInstance()->trumptToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), player->pid);
			}

			player->calcMaxWin();
			table->calcMaxValue(player);
			updateDB_UserCoin(player, table);
		}
		//处理在观战的玩家发魔法表情，然后扣除金币的问题
		if(player && player->isReady())
		{
			if(player->m_nMagicNum != 0 && player->m_nMagicCoinCount != 0)
			{
				if(player->m_lMoney - player->m_lCarryMoney >= player->m_nMagicCoinCount)
				{
					player->m_lMoney -= player->m_nMagicCoinCount;
				}
				else
				{
					player->m_lCarryMoney -= player->m_nMagicCoinCount - (player->m_lMoney - player->m_lCarryMoney);
					player->m_lMoney -= player->m_nMagicCoinCount;
				}
				updateDB_UserCoin(player, table, player->m_nMagicCoinCount);
			}
		}
	}

	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["Time"]=(int)(time(NULL) - table->getStartTime());
	data["num"] = table->m_bMaxNum;
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
		char szplayer[16] = {0};
		sprintf(szplayer, "uid%d",i);
		if(otherplayer)
		{
			sendGameOverinfo(otherplayer, table);
			data[szplayer] = otherplayer->id;
			sprintf(szplayer, "fvalue%d",i);
			data[szplayer] = otherplayer->m_nEndCardType;
			sprintf(szplayer, "fcoin%d",i);
			data[szplayer] = (double)otherplayer->m_lFinallGetCoin;
			sprintf(szplayer, "carrymoney%d",i);
			data[szplayer] = (double)otherplayer->m_lCarryMoney;
			sprintf(szplayer, "money%d",i);
			data[szplayer] = (double)otherplayer->m_lMoney;
		}
		else
			data[szplayer] = 0;
	}

	if(!table->isAllRobot())
		_LOG_REPORT_(table->m_nRaseUid, RECORD_GAME_OVER, "%s", data.toStyledString().c_str());

	
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* otherplayer = table->player_array[i];
		//当前这个用户已经掉线
		if(otherplayer && otherplayer->isdropline)
		{
			serverPushLeaveInfo(table, otherplayer);
			table->playerLeave(otherplayer);
		}
		//用户下低注超时超过几次直接踢出用户
		if(otherplayer && (otherplayer->timeoutCount >= Configure::getInstance()->timeoutCount))
		{
			serverPushLeaveInfo(table, otherplayer, 3);
			table->playerLeave(otherplayer);
		}
	}
	
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* otherplayer = table->player_array[i];
		if(otherplayer && otherplayer->m_nBuyInFlag == 1)
		{
			if(otherplayer->m_lCarryMoney < otherplayer->coincfg.carrycoinmax)
			{
				if(otherplayer->coincfg.carrycoinmax <= otherplayer->m_lMoney)
					otherplayer->m_lCarryMoney = otherplayer->coincfg.carrycoinmax;
				else
				{
					otherplayer->m_lCarryMoney = otherplayer->m_lMoney;
				}
			}
		}
	}
	
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* otherplayer = table->player_array[i];
		if(otherplayer && otherplayer->m_lAddMoney != 0 && otherplayer->m_nBuyInFlag != 1)
		{
			if(otherplayer->m_lCarryMoney < otherplayer->coincfg.carrycoinmax)
			{
				if(otherplayer->m_lCarryMoney + otherplayer->m_lAddMoney <= otherplayer->m_lMoney)
				{
					otherplayer->m_lCarryMoney += otherplayer->m_lAddMoney;
					if(otherplayer->m_lCarryMoney > otherplayer->coincfg.carrycoinmax)
						otherplayer->m_lCarryMoney = otherplayer->coincfg.carrycoinmax;
				}
			}
			otherplayer->m_lAddMoney = 0;
		}
	}
	return 0;
}


int IProcess::sendGameOverinfo(Player* player, Table* table)
{
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	int i = 0;
	int j = 0;

	OutputPacket response;
	response.Begin(GMSERVER_MSG_GAMEOVER,player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_nCountPlayer);
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteShort(otherplayer->m_nStatus);
			response.WriteByte(otherplayer->m_nTabIndex);
			response.WriteByte(otherplayer->m_nEndCardType);
			response.WriteInt64(otherplayer->m_lFinallGetCoin);
			response.WriteInt64(otherplayer->m_lMoney);
			response.WriteInt64(otherplayer->m_lCarryMoney);
			response.WriteInt(otherplayer->m_nWin);
			response.WriteInt(otherplayer->m_nLose);
			response.WriteByte(otherplayer->m_setPoolResult.size());
			for (std::set<int>::iterator it=otherplayer->m_setPoolResult.begin(); it!=otherplayer->m_setPoolResult.end(); ++it)
			{
				response.WriteByte((BYTE)(*it));
				response.WriteInt64(otherplayer->m_PoolNumArray[*it]);
				_LOG_DEBUG_("uid:%d poolname:%d getCountCoin:%d\n", otherplayer->id, *it, otherplayer->m_PoolNumArray[*it]);
			}
		}
	}
	response.WriteByte(player->m_bCompleteTask ? 1 : 0);
	response.WriteShort(player->m_bCompleteTask ? player->m_nGetRoll : 0);
	response.WriteInt(player->m_nRoll);
	response.End();

	_LOG_INFO_("[GameOverinfo] Push [0x%04x] to uid=[%d] m_lFinallGetCoin[%ld] m_lMoney[%ld]  CompleteTask[%d]\n", 
		GMSERVER_MSG_GAMEOVER, player->id,player->m_lFinallGetCoin, player->m_lMoney, player->m_bCompleteTask ? 1 : 0);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_nEndCardType[%d]\n",player->m_bHasCard ? player->m_nEndCardType : 0);
	_LOG_DEBUG_("[Data Response] m_lMoney[%d]\n",player->m_lMoney);
	_LOG_DEBUG_("[Data Response] m_lCarryMoney[%d]\n",player->m_lCarryMoney);
	_LOG_DEBUG_("[Data Response] m_nWin[%d]\n",player->m_nWin);
	_LOG_DEBUG_("[Data Response] m_nLose[%d]\n",player->m_nLose);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] m_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] leavercount=[%d]\n", table->leavercount);
	_LOG_DEBUG_("[Data Response] m_nRoll=[%d]\n", player->m_nRoll);
	_LOG_DEBUG_("[Data Response] poolcount=[%d]\n", player->m_setPoolResult.size());


	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendRoundCard] Send To Uid[%d] Error!\n", player->id);
	return 0;
}

int IProcess::serverPushLeaveInfo(Table* table, Player* leavePlayer,short retno)
{
	if(table == NULL || leavePlayer == NULL)
		return -1;
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	
	_LOG_INFO_("<==[serverPushLeaveInfo] Push [0x%04x] leaver[%d] retno[%d]\n", SERVER_MSG_KICKOUT, leavePlayer->id, retno);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *other = NULL;
		if (i < GAME_PLAYER)
			other = table->player_array[i];
		else {
			other = *it;
			it++;
		}
		if(other)
		{
			OutputPacket response;
			response.Begin(SERVER_MSG_KICKOUT,other->id);
			response.SetSource(E_MSG_SOURCE_GAME_SERVER);
			response.WriteShort(retno);
			response.WriteString(ErrorMsg::getInstance()->getErrMsg(retno));
			response.WriteInt(other->id);
			response.WriteShort(other->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(leavePlayer->id);
			response.End();
			HallManager::getInstance()->sendToHall(other->m_nHallid, &response, false);
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] leavePlayer=[%d]\n", leavePlayer->id);
	_LOG_DEBUG_("[Data Response] leavePlayerstatus=[%d]\n", leavePlayer->m_nStatus);

	return 0;
}

int IProcess::serverWarnPlayerKick(Table* table, Player* warnner, short timeCount)
{
	if(table == NULL || warnner == NULL)
		return -1;
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	
	_LOG_DEBUG_("<==[serverWarnPlayerKick] Push [0x%04x]\n", GMSERVER_WARN_KICK);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int sendnum = 0;
	int i = 0;
	OutputPacket response;
	response.Begin(GMSERVER_WARN_KICK,warnner->id);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(warnner->id);
	response.WriteShort(warnner->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteShort(timeCount);
	response.End();
	HallManager::getInstance()->sendToHall(warnner->m_nHallid, &response, false);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] warnner=[%d]\n", warnner->id);
	_LOG_DEBUG_("[Data Response] timeCount=[%d]\n", timeCount);

	return 0;
}

int IProcess::updateDB_UserCoin(Player* player, Table* table)
{
	if(table->isAllRobot())
		return 0;
	if(player->source != 30)
	{
		OutputPacket respone;
		respone.Begin(UPDATE_MONEY);
		respone.WriteInt(player->id); 
		respone.WriteByte(1);  
		respone.WriteInt64(player->m_lFinallGetCoin /*- table->m_lTax*/ - player->m_nMagicCoinCount);
		respone.End();
		if(MoneyServer()->Send(&respone) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );

		OutputPacket outPacket;
		outPacket.Begin(ROUND_SET_INFO);
		outPacket.WriteInt(player->id); 
		outPacket.WriteInt(player->m_nWin);
		outPacket.WriteInt(player->m_nLose);
		outPacket.WriteInt(player->m_nTie);
		outPacket.WriteInt(player->m_nRunAway);
		outPacket.WriteInt64(player->m_lMaxWinMoney);
		outPacket.WriteInt64(player->m_lMaxCardValue);
		outPacket.End();
		if(RoundServer()->Send( &outPacket ) < 0)
			_LOG_ERROR_("Send request to RoundServerConnect Error\n" );
	}

	int cmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(cmd);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt64(player->m_lFinallGetCoin - table->m_lTax);
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(player->m_nTax); 
	outputPacket.WriteInt(table->m_lBigBlind);  
	outputPacket.WriteInt(Configure::getInstance()->server_id);  
	outputPacket.WriteInt(table->id);
	outputPacket.WriteInt(table->m_nType);
	outputPacket.WriteString(table->getGameID());  
	//outputPacket.WriteInt(table->getEndType());
    outputPacket.WriteInt(player->isdropline?GAME_END_DISCONNECT:GAME_END_NORMAL);
	outputPacket.WriteInt64(player->m_lMoney); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(table->getEndTime());  
	outputPacket.WriteInt(player->m_nEndCardType);
	outputPacket.WriteInt(0);//taskcoin
	outputPacket.WriteInt(player->m_nMagicNum); 
	outputPacket.WriteInt(player->m_nMagicCoinCount); 
	outputPacket.End();

	_LOG_DEBUG_("==>[updateDB] [0x%04x] [updateDB_UserCoin]\n", cmd);

	if(MySqlServer()->Send( &outputPacket )==0)
	{
		_LOG_DEBUG_("Transfer request to BackServer OK\n" );
		return 0;
	}
	else
	{
		 _LOG_ERROR_("Send request to BackServer Error\n" );
		 return -1;
	}
	return 0;
}

int IProcess::updateDB_UserCoin(Player* player, Table* table, int MagicSubCoin)
{
	if(player->source != 30)
	{
		OutputPacket respone;
		respone.Begin(UPDATE_MONEY);
		respone.WriteInt(player->id); 
		respone.WriteByte(1);  
		respone.WriteInt64(-player->m_nMagicCoinCount);
		respone.End();
		if(MoneyServer()->Send(&respone) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );
	}

	int cmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(cmd);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt64(0);
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(0); 
	outputPacket.WriteInt(table->m_lBigBlind);  
	outputPacket.WriteInt(Configure::getInstance()->server_id);  
	outputPacket.WriteInt(table->id);  
	short m_nType = table->m_nType;
	//if(table->m_nType == 6 || table->m_nType == 7)
	//	m_nType = 3;
	outputPacket.WriteInt(m_nType);  
	outputPacket.WriteString(table->getGameID());  
	//outputPacket.WriteInt(table->getEndType());  
    outputPacket.WriteInt(player->isdropline?GAME_END_DISCONNECT:GAME_END_NORMAL);
	outputPacket.WriteInt64(player->m_lMoney); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(time(NULL));  
	outputPacket.WriteInt(0);
	outputPacket.WriteInt(0);//taskcoin
	outputPacket.WriteInt(player->m_nMagicNum); 
	outputPacket.WriteInt(player->m_nMagicCoinCount); 
	outputPacket.End();

	_LOG_DEBUG_("==>[updateDB] [0x%04x] [updateDB_UserCoin]\n", cmd);

	if(MoneyServer()->Send( &outputPacket )==0)
	{
		_LOG_DEBUG_("Transfer request to BackServer OK\n" );
		return 0;
	}
	else
	{
		 _LOG_ERROR_("Send request to BackServer Error\n" );
		 return -1;
	}
	return 0;
}

int IProcess::updateDB_UserCoin(LeaverInfo* player, Table* table)
{
	if(table->isAllRobot())
		return 0;
	if(player->source != 30)
	{
		_LOG_DEBUG_("before m_nMagicCoinCount:%d m_lMoney:%d m_nMagicNum:%d \n", player->m_nMagicCoinCount, player->m_lMoney, player->m_nMagicNum);
		if(player->m_lMoney - player->m_nMagicCoinCount < 0)
		{
			player->m_nMagicCoinCount = player->m_lMoney;
			player->m_lMoney = 0;
		}
		OutputPacket respone;
		respone.Begin(UPDATE_MONEY);
		respone.WriteInt(player->uid); 
		respone.WriteByte(1); 
		respone.WriteInt64(-player->betcoin - table->m_lTax - player->m_nMagicCoinCount);
		respone.End();
		if(MoneyServer()->Send(&respone) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );

		OutputPacket outPacket;
		outPacket.Begin(ROUND_SET_INFO);
		outPacket.WriteInt(player->uid); 
		outPacket.WriteInt(player->m_nWin);
		outPacket.WriteInt(player->m_nLose + 1);
		outPacket.WriteInt(player->m_nTie);
		outPacket.WriteInt(player->m_nRunAway);
		outPacket.WriteInt64(player->m_lMaxWinMoney);
		outPacket.WriteInt64(player->m_lMaxCardValue);
		outPacket.End();
		if(RoundServer()->Send( &outPacket ) < 0)
			_LOG_ERROR_("Send request to RoundServerConnect Error\n" );
		_LOG_DEBUG_("after m_nMagicCoinCount:%d m_lMoney:%d m_nMagicNum:%d \n", player->m_nMagicCoinCount, player->m_lMoney, player->m_nMagicNum);
	}


	int sedcmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(sedcmd);
	outputPacket.WriteInt(player->uid);  
	outputPacket.WriteInt64(-player->betcoin - table->m_lTax);
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(table->m_lTax); 
	outputPacket.WriteInt(table->m_lBigBlind);  
	outputPacket.WriteInt(Configure::getInstance()->server_id);  
	outputPacket.WriteInt(table->id); 
	//if(table->m_nType == 6 || table->m_nType == 7)
	//	m_nType = 3;
	outputPacket.WriteInt(table->m_nType);  
	outputPacket.WriteString(table->getGameID());  
	//outputPacket.WriteInt(table->getEndType());  
    outputPacket.WriteInt(GAME_END_DISCONNECT);
	outputPacket.WriteInt64(player->m_lMoney); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(time(NULL));  
	outputPacket.WriteInt(-1); 
	outputPacket.WriteInt(0); 
	outputPacket.WriteInt(player->m_nMagicNum); 
	outputPacket.WriteInt(player->m_nMagicCoinCount); 
	outputPacket.End();

	_LOG_DEBUG_("==>[updateDB] [0x%04x] [updateDB_UserCoin]\n", sedcmd);

	if(MySqlServer()->Send( &outputPacket )==0)
	{
		_LOG_DEBUG_("Transfer request to BackServer OK\n" );
		return 0;
	}
	else
	{
		 _LOG_ERROR_("Send request to BackServer Error\n" );
		 return -1;
	}
	return 0;
}

int IProcess::UpdateDBActiveTime(Player* player)
{
	if(player->source == 30)
		return 0;
	if(time(NULL) - player->getEnterTime() > 36000)
        return 0;
	OutputPacket outputPacket;
	outputPacket.Begin(ROUND_SET_ONLINETIME);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt(time(NULL) - player->getEnterTime());
	outputPacket.End();	
 
	_LOG_DEBUG_("==>[updateDB] [0x%04x] [UpdateDBActiveTime]\n", ROUND_SET_ONLINETIME);
 
	if(RoundServer()->Send( &outputPacket )<0)
		 _LOG_ERROR_("Send request to RoundServerConnect Error\n" );
	return 0;
}

int IProcess::sendTakeSeatInfo(Table* table, Player *player, int action, int seatid, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = svid << 16|table->id;
	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *other = NULL;
		if (i < GAME_PLAYER)
			other = table->player_array[i];
		else {
			other = *it;
			it++;
		}
		if(other)
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_TAKESEAT, other->id);
			if(other->id == player->id)
				response.SetSeqNum(seq);
			response.WriteShort(0);
			response.WriteString("ok");
			response.WriteInt(other->id);
			response.WriteShort(other->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(player->id);
			response.WriteString(player->name);
			response.WriteInt64(player->m_lMoney);
			response.WriteString(player->json);
			response.WriteByte(action);
			response.WriteByte(seatid);
			response.End();
			LOGGER(E_LOG_INFO) << "send sendTakeSeatInfo msg to player = " << other->id << " seatid:" << other->m_nTabIndex;
			HallManager::getInstance()->sendToHall(other->m_nHallid, &response, false);
		}
	}

	_LOG_DEBUG_("<==[sendTakeSeatInfo] Push [0x%04x]\n", CLIENT_MSG_TAKESEAT);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);

	return 0;

}


void IProcess::sendChatInfo(Table* table, Player* player, int touid, short type, const char* msg, short seq)
{
    _LOG_DEBUG_("<==[sendChatInfo] Push [0x%04x]\n", CLIENT_MSG_CHAT);
    int svid = Configure::getInstance()->m_nServerId;
    int tid = svid << 16|table->id;
    PlayerSet::iterator it = table->m_Viewers.begin();
    for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
    {
        Player *other = NULL;
        if (i < GAME_PLAYER)
            other = table->player_array[i];
        else {
            other = *it;
            it++;
        }
        if(other)
        {
            OutputPacket response;
            response.Begin(CLIENT_MSG_CHAT, other->id);
            if (other->id == player->id)
                response.SetSeqNum(seq);

            response.WriteShort(0);
            response.WriteString("");
            response.WriteInt(other->id);
            response.WriteShort(other->m_nStatus);
            response.WriteInt(tid);
            response.WriteShort(table->m_nStatus);
            response.WriteInt(player->id);
            response.WriteString(player->name);
            response.WriteString(player->json);
            response.WriteInt(touid);
            response.WriteByte(type);
            response.WriteString(msg);
            response.End();
            HallManager::getInstance()->sendToHall(other->m_nHallid, &response, false);
        }
    }
}

void IProcess::sendGiftInfo(Table* table, Player* player, int toseatid, short type, int subid, BYTE sendtoall,short seq)
{
	_LOG_DEBUG_("<==[sendChatInfo] Push [0x%04x]\n", CLIENT_MSG_SEND_GIFT);
	int svid = Configure::getInstance()->m_nServerId;
	int tid = svid << 16|table->id;
    int numplayer = 0;
    for(int j = 0; j < GAME_PLAYER; j++) {
        if (table->player_array[j])
            numplayer++;
    }
	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *other = NULL;
		if (i < GAME_PLAYER)
			other = table->player_array[i];
		else {
			other = *it;
			it++;
		}
		if(other)
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_SEND_GIFT, other->id);
			if (other->id == player->id)
				response.SetSeqNum(seq);

			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(player->m_nTabIndex);
            response.WriteByte(sendtoall);
            if (!sendtoall) {
                response.WriteInt(1);

                response.WriteInt(toseatid);
            } else {
                response.WriteInt(numplayer - 1);
                for(int j = 0; j < GAME_PLAYER; j++) {
                    if (table->player_array[j])
                        response.WriteInt(table->player_array[j]->m_nTabIndex);
                }
            }
            response.WriteShort(type);
            response.WriteInt(subid);
			response.End();
			HallManager::getInstance()->sendToHall(other->m_nHallid, &response, false);
		}
	}
}

void IProcess::sendUpdateMoney(Table* table, Player* player)
{
    _LOG_DEBUG_("<==[sendChatInfo] Push [0x%04x]\n", CLIENT_MSG_GET_MONEY);
    int svid = Configure::getInstance()->m_nServerId;
    int tid = svid << 16|table->id;
    PlayerSet::iterator it = table->m_Viewers.begin();
    for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
    {
        Player *other = NULL;
        if (i < GAME_PLAYER)
            other = table->player_array[i];
        else {
            other = *it;
            it++;
        }
        if(other)
        {
            OutputPacket response;
            response.Begin(CLIENT_MSG_GET_MONEY, other->id);
            response.WriteInt(player->id);
            response.WriteInt64(player->m_lMoney);
            response.WriteShort(1);
            response.End();
            HallManager::getInstance()->sendToHall(other->m_nHallid, &response, false);
        }
    }
}
