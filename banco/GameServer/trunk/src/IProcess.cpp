#include "IProcess.h"
#include "Configure.h"
#include "HallManager.h"
#include "Logger.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "CoinConf.h"
#include "ProtocolServerId.h"
#include "Util.h"
#include "Player.h"
#include "RedisLogic.h"
#include "GameApp.h"
#include "MySqlAgent.h"
#include "RoundAgent.h"
#include "MoneyAgent.h"
#include "GameUtil.h"

int IProcess::sendTableStatus(Table* table, short ntime)
{
	if(table == NULL)
		return -1;
	
	CoinConf::getInstance()->setTableStatus(table->m_nStatus);

	if(table->m_nStatus == STATUS_TABLE_IDLE)
	{
		table->HandleBankeList();

		//处理用户掉线的情况
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			Player* player = table->player_array[i];
			if(player)
			{
				bool bleave = false;
				if((!player->isonline)&&(player->notBetCoin() && table->bankeruid != player->id))
					bleave = true;
				//处理用户半天不下注，直接踢出服务器
				if(player->source != E_MSG_SOURCE_ROBOT && player->id != table->bankeruid && !table->isWaitForBanker(player->id) &&
					(time(NULL) - player->m_nLastBetTime) > Configure::getInstance()->kicktime)
				{
                    ULOGGER(E_LOG_INFO, player->id) << " over kicktime = " << Configure::getInstance()->kicktime;
					bleave = true;
				}

				if(bleave)
				{
                    serverPushLeaveInfo(table,player,1);
					if(player->id == table->bankeruid)
					{
						//莊掉线直接游戏结束轮换荘家
						table->rotateBanker();
					}
					table->playerLeave(player);
				}
			}
		}

        Player* banker = table->getBanker();
		if(banker && banker->source != E_MSG_SOURCE_ROBOT)
		{
			if((banker->m_lMoney < table->m_nBankerlimit) || (table->bankernum >= table->m_nMaxBankerNum) || (banker->isCanlcebanker))
			{
				banker->isCanlcebanker = false;
				table->rotateBanker();
			}
		}
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(GMSERVER_MSG_TABSTATUS, table->player_array[i]->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(table->id);
			response.WriteShort(table->m_nStatus);
			response.WriteByte(ntime);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);

			LOGGER(E_LOG_INFO) << "refresh_table_status, cmd = " << TOHEX(GMSERVER_MSG_TABSTATUS)
				<< " uid = " << table->player_array[i]->id
				<< " ustate = " << table->player_array[i]->m_nStatus
				<< " tid = " << table->id
				<< " tstate = " << table->m_nStatus
				<< " time = " << ntime;
		}
	}

    // 输出调用堆栈
	//if (table->m_nStatus == STATUS_TABLE_SETTLEMENT)
	//{
	//	GameUtil::printStackTrace();
	//}
	
	return 0;
}

int IProcess::NoteBankerLeave(Table* table, Player* banker)
{
	if (table== NULL || banker == NULL)
	{
		return -1;
	}
	
	_LOG_DEBUG_("<==[NoteBankerLeave] Push [0x%04x]\n", GMSERVER_MSG_NOTELEAVE);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = table->player_array[i];
		if(player)
		{
			OutputPacket response;
			response.Begin(GMSERVER_MSG_NOTELEAVE, player->id);
			response.WriteShort(0);
			response.WriteString(ErrorMsg::getInstance()->getErrMsg(-17));
			response.WriteInt(player->id);
			response.WriteShort(player->m_nStatus);
			response.WriteInt(table->id);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(banker->id);
			response.WriteShort(banker->m_nSeatID);
			response.End();
			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
		}
	}
	return 0;
}

int IProcess::rotateBankerInfo(Table* table, Player* banker, Player* prebanker)
{
	if (table== NULL)
	{
		return -1;
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = table->player_array[i];
		if(player)
		{
			OutputPacket response;
			response.Begin(GMSERVER_MSG_ROTATEBANKE, player->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(player->id);
			response.WriteShort(player->m_nStatus);
			response.WriteInt(table->id);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(prebanker ? prebanker->id : 0);
			response.WriteShort(prebanker ? prebanker->m_nSeatID : -1);
			response.WriteInt(banker ? banker->id : 0);
			response.WriteShort(banker ? banker->m_nSeatID : -1);
			response.WriteInt64(banker ? banker->m_lMoney : 0);
			response.WriteString(banker ? banker->name : "");
			response.WriteString(banker ? banker->json : "");
			response.WriteByte((BYTE)(table->m_nMaxBankerNum - table->bankernum));
			response.WriteByte(table->isWaitForBanker(player->id) ? 1: 0);
			response.End();
			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
		}
	}

	LOGGER(E_LOG_DEBUG) << "tid = " << table->id << " table status = " << table->m_nStatus <<
		" prebanker = " << (prebanker ? prebanker->id : 0) << " prebanker seatid = " << (prebanker ? prebanker->m_nSeatID : -1) <<
		" banker = " << (banker ? banker->id : 0) << " banker seatid = " << (banker ? banker->m_nSeatID : -1) <<
		" banker money = " << (banker ? banker->m_lMoney : 0) << " banker name = " << (banker ? banker->name : "") <<
		" banker json = " << (banker ? banker->json : "");

	CoinConf::getInstance()->setTableBanker(banker ? banker->id : 0);

	//if(banker)
	//{
	//	char buff[128] = {0};
	//	int num = 5 + rand()%5;
	//	sprintf(buff, "【系统】:%s%s",banker->name, ErrorMsg::getInstance()->getErrMsg(num));
	//	AllocSvrConnect::getInstance()->trumptToUser(6,buff);
	//}

	return 0;
}

int IProcess::serverPushLeaveInfo(Table* table, Player* leavePlayer,short retno)
{
	if(table == NULL || leavePlayer == NULL)
		return -1;
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	
	//_LOG_DEBUG_("<==[serverPushLeaveInfo] Push [0x%04x]\n", SERVER_MSG_KICKOUT);
	//_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(SERVER_MSG_KICKOUT,table->player_array[i]->id);
			response.SetSource(E_MSG_SOURCE_GAME_SERVER);
			response.WriteShort(retno);
			response.WriteString(ErrorMsg::getInstance()->getErrMsg(retno));
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(leavePlayer->id);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
		}
	}

	return 0;
}

int IProcess::GameStart(Table* table)
{
	if (table == NULL)
		return -1;
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16) | table->id;

	LOGGER(E_LOG_DEBUG) << "Push " << GMSERVER_GAME_START;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		Player *otherplayer = table->player_array[i];
		if (otherplayer)
		{
			OutputPacket response;
			response.Begin(GMSERVER_GAME_START, otherplayer->id);
			response.WriteShort(0);
			response.WriteString("ok");
			response.WriteInt(otherplayer->id);
			response.WriteShort(otherplayer->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(table->bankeruid);
			response.WriteByte(Configure::getInstance()->idletime);
			response.WriteByte(table->getPlayerCount(false));
			for (int j = 0; j < GAME_PLAYER; ++j)
			{
				if (table->player_array[j])
				{
					response.WriteInt(table->player_array[j]->id);
					response.WriteShort(table->player_array[j]->m_nStatus);
					response.WriteByte(table->player_array[j]->m_nSeatID);
				}
			}
			response.WriteInt(table->coincfg.ante);
			response.End();
			HallManager::getInstance()->sendToHall(otherplayer->m_nHallid, &response, false);
		}
	}

	LOGGER(E_LOG_DEBUG) << "tid = " << tid << " status = " << table->m_nStatus;
	return 0;
}

int IProcess::GameOver(Table* table)
{
	int i,j;
	Player* banker = NULL;
	int64_t maxWinCoin = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = table->player_array[i];
		
		if(player)
		{
			int64_t winmoney = player->m_lWinScore + player->m_lLostScore;
			if(maxWinCoin < winmoney)
			{
				maxWinCoin = winmoney;
				table->maxwinner = player;
			}
			if(player->m_lWinScore + player->m_lLostScore > 0)
			{
				player->m_nWin ++;
			}
			else if (player->m_lWinScore + player->m_lLostScore < 0)
			{
				player->m_nLose ++;
			}
			/*if(player->m_nChatCostCount != 0 || ((player->m_lWinScore + player->m_lLostScore) != 0))*/
				updateDB_UserCoin(player, table);
			//结束的话就清空聊天消耗
			player->m_nChatCostCount = 0;
		}
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = table->player_array[i];
		
		if(player)
		{
			OutputPacket response;
			response.Begin(GMSERVER_MSG_GAMEOVER, player->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(player->id);
			response.WriteShort(player->m_nStatus);
			response.WriteInt(table->id);
			response.WriteShort(table->m_nStatus);
            for (int k = 0; k < 2; k++)
            {
                int count = table->m_cbCardCount[k];
                response.WriteInt(count);
                for (int j = 0; j < count; j++)
                {
					LOGGER(E_LOG_INFO/*, player->id*/) << "TableCardArray" << "[" << k << "]" << "[" << j << "]" << table->m_cbTableCardArray[k][j];
                    response.WriteByte(table->m_cbTableCardArray[k][j]);
                }
            }
			response.WriteByte(table->m_bWinType);
			response.WriteInt64(player->m_lWinScore + player->m_lLostScore);
			response.WriteInt64(player->m_lReturnScore);
			response.WriteInt64(player->m_lMoney);
			response.WriteByte((BYTE)(table->m_nMaxBankerNum - table->bankernum));
			response.WriteInt64(table->m_lBankerWinScoreCount);
			banker = table->getBanker();
			response.WriteInt64(banker ? banker->m_lMoney : 0);
			response.WriteInt(table->maxwinner ? table->maxwinner->id : 0);
			response.WriteString(table->maxwinner ? table->maxwinner->name : "name");
			response.WriteInt64(table->maxwinner ? table->maxwinner->m_lWinScore - table->maxwinner->m_lBetArray[0]: 0);
			response.WriteInt(table->player_array[i]->m_nWin);
			response.WriteInt(table->player_array[i]->m_nLose);
            response.WriteByte(table->m_cbBankerPoint);
            response.WriteByte(table->m_cbPlayerPoint);
            response.WriteByte(table->m_cbBankerPair);
            response.WriteByte(table->m_cbPlayerPair);
            response.WriteInt(table->m_RemainCardCount);
            response.WriteInt(CGameLogic::GetCardCount());
			response.End();
            LOGGER(E_LOG_INFO/*, player->id*/) << "win score = " << player->m_lWinScore << " BankerPoint = " 
				<< table->m_cbBankerPoint << "  PlayerPoint = " << table->m_cbPlayerPoint;
			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
		}
	}

	ResetAllPlayerState(table, STATUS_PLAYER_OVER);

	return 0;
}

int IProcess::updateDB_UserCoin(Player* player, Table* table)
{
	if(player->source != 30)
	{
		OutputPacket respone;
		respone.Begin(UPDATE_MONEY);
		respone.WriteInt(player->id); 
		respone.WriteByte(1);
		respone.WriteInt64(player->m_lWinScore + player->m_lLostScore - player->m_nChatCostCount);
		respone.End();
		if (MoneyServer()->Send(&respone) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n");

		OutputPacket outPacket;
		outPacket.Begin(ROUND_SET_INFO);
		outPacket.WriteInt(player->id); 
		outPacket.WriteInt(player->m_nWin );
		outPacket.WriteInt(player->m_nLose);
		outPacket.WriteInt(player->m_nTie);
		outPacket.WriteInt(player->m_nRunAway);
		outPacket.End();
		if (RoundServer()->Send(&outPacket) < 0)
			_LOG_ERROR_("Send request to RoundServerConnect Error\n");
		
		int cmd = 0x0120;
		OutputPacket outputPacket;
		outputPacket.Begin(cmd);
		outputPacket.WriteInt(player->id);  
		outputPacket.WriteInt64(player->m_lWinScore + player->m_lLostScore);
		outputPacket.WriteInt(player->source);  
		outputPacket.WriteInt(player->cid);  
		outputPacket.WriteInt(player->sid);  
		outputPacket.WriteInt(player->pid);  
		outputPacket.WriteInt(player->tax); //table->m_nTax
		outputPacket.WriteInt(0);			//table->m_nAnte;
		outputPacket.WriteInt(Configure::getInstance()->m_nServerId);  
		outputPacket.WriteInt(table->id); 
		outputPacket.WriteInt(Configure::getInstance()->m_nLevel);
		outputPacket.WriteString(table->getGameID());  
		outputPacket.WriteInt(table->m_bWinType);  
		outputPacket.WriteInt64(player->m_lMoney); 
		outputPacket.WriteInt(table->getStartTime());  
		outputPacket.WriteInt(table->getEndTime());
		outputPacket.WriteInt(player->m_bisCall);			//是否抢庄
// 		outputPacket.WriteInt(player->m_nChatCostCount);
// 		outputPacket.WriteInt(0);
// 		outputPacket.WriteInt(0);
// 		outputPacket.WriteInt(0);
// 		outputPacket.WriteInt(0);
		outputPacket.End();

		if (MySqlServer()->Send(&outputPacket) >= 0)
		{
			return 0;
		}
		else
		{
			_LOG_ERROR_("Send request to BackServer Error\n");
			return -1;
		}
	}
	
	return 0;
}


int IProcess::UpdateDBActiveTime(Player* player)
{
	if(player->source == 30)
		return 0;
	OutputPacket outputPacket;
	outputPacket.Begin(ROUND_SET_ONLINETIME);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt(time(NULL) - player->getEnterTime());
	outputPacket.End();	
 
	_LOG_INFO_("==>[updateDB] [0x%04x] [UpdateDBActiveTime]\n", ROUND_SET_ONLINETIME);
 
	if (RoundServer()->Send(&outputPacket) >= 0)
	{
		_LOG_DEBUG_("Transfer request to RoundServerConnect OK\n");
		return 0;
	}
	else
	{
		_LOG_ERROR_("Send request to RoundServerConnect Error\n");
		return -1;
	}
	return 0;
}

int IProcess::NotifyGameResult(Table * table)
{
	if (table == NULL)
	{
		return -1;
	}
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* p = table->player_array[i];
		if (p == NULL) continue;

		OutputPacket response;
		response.Begin(SERVER_NOTIFY_GAMERESULT, p->id);
		response.WriteShort(0);
		response.WriteString("");
		response.WriteInt(p->id);
		response.WriteInt(table->getPlayerCount(false));
		for (int j = 0; j < GAME_PLAYER; j++)
		{
			Player* player = table->player_array[j];
			if (player == NULL) continue;
			response.WriteInt(player->m_nSeatID);
			response.WriteInt64(player->m_lWinScore);
			response.WriteInt64(player->m_lMoney);
		}
		response.End();
		HallManager::getInstance()->sendToHall(p->m_nHallid, &response, false);
	}
	return 0;
}

int IProcess::sendTakeSeatInfo(Table* table, Player *player, int action, int seatid, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = svid << 16 | table->id;

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		Player *other = table->player_array[i];
		if (other == NULL)
		{
			LOGGER(E_LOG_INFO) << "index of << " << i << " in player_array is invalid!";
			continue;
		}

		OutputPacket response;
		response.Begin(CLIENT_MSG_STANDUP, other->id);
		if (other->id == player->id)
			response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(other->id);
		response.WriteShort(other->m_nStatus);
		response.WriteInt(tid);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(player->id);
		response.WriteShort(player->m_nStatus);
		response.WriteString(player->name);
		response.WriteInt64(player->m_lMoney);
		response.WriteString(player->json);
		response.WriteByte(action);
		response.WriteByte(seatid);
		response.End();
		LOGGER(E_LOG_INFO) << "send sendTakeSeatInfo msg to player = " << other->id;
		HallManager::getInstance()->sendToHall(other->m_nHallid, &response, false);
	}

	return 0;
}

void IProcess::sendChatInfo(Table* table, Player* player, int touid, short type, const char* msg, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = svid << 16 | table->id;

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		Player *other = table->player_array[i];
		if (other == NULL)
		{
			LOGGER(E_LOG_INFO) << "index of << " << i << " in player_array is invalid!";
			continue;
		}

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
		LOGGER(E_LOG_INFO) << "send chat info, cmd = " << TOHEX(CLIENT_MSG_CHAT);
		HallManager::getInstance()->sendToHall(other->m_nHallid, &response, false);
	}
}

bool IProcess::CanPlay(Player* player)
{
	if (player == NULL)
	{
		return false;
	}

	short tmpStatus = player->m_nStatus;
	if (STATUS_PLAYER_STANDUP == tmpStatus || STATUS_PLAYER_BANKING == tmpStatus)
	{
		return false;
	}

	return true;
}

bool IProcess::ResetAllPlayerState(Table * pTable, int destStatus)
{
	if (pTable == NULL)
	{
		return false;
	}

	for (int idx = 0; idx < GAME_PLAYER; ++idx)
	{
		Player *pCur = pTable->player_array[idx];
		if (!CanPlay(pCur))
		{
			continue;
		}

		pCur->m_nStatus = destStatus;
	}

	return true;
}

int IProcess::sendTabePlayersInfo(Table* table, Player* player, int comeUid, short seq)
{
	if (table == NULL)
	{
		return -2;
	}

	if (player == NULL)
	{
		return -1;
	}

	LOGGER(E_LOG_INFO) << " cmd = " << TOHEX(CLIENT_MSG_LOGINCOMING)
		<< " dest_uid = " << player->id
		<< " table_id = " << table->id
		<< " come_uid = " << comeUid
		<< " seq = " << seq;

	short timeout = 0;
	if (table->isIdle())
	{
		timeout = Configure::getInstance()->idletime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isBet())
	{
		timeout = Configure::getInstance()->bettime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isOpen())
	{
		timeout = Configure::getInstance()->opentime - (time(NULL) - table->getStatusTime());
	}

	OutputPacket response;
	response.Begin(CLIENT_MSG_LOGINCOMING, player->id);
	{
		if (player->id == comeUid)
			response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("ok");
		response.WriteInt(player->id);
		response.WriteShort(player->m_nStatus);
		response.WriteInt(Configure::getInstance()->m_nServerId << 16 | table->id);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(comeUid);		// 新加入的玩家ID
		response.WriteShort(table->getPlayerCount(false));
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			Player* getPlayer = table->player_array[i];
			if (getPlayer == NULL)
			{
				continue;
			}

			response.WriteInt(getPlayer->id);
			response.WriteString(getPlayer->name);
			response.WriteShort(getPlayer->m_nStatus);
			response.WriteByte(getPlayer->m_nSeatID);
			response.WriteInt64(getPlayer->m_lMoney);
			//说明当前正在发牌倒计时中
			response.WriteInt(getPlayer->m_nWin);
			response.WriteInt(getPlayer->m_nLose);
			response.WriteString(getPlayer->json);
			ULOGGER(E_LOG_INFO, getPlayer->id) << " id = " << getPlayer->id
				<< " name = " << getPlayer->name
				<< " m_nStatus = " << getPlayer->m_nStatus
				<< " m_nSeatID = " << getPlayer->m_nSeatID
				<< " json = " << getPlayer->json;
		}
		response.WriteShort(player->m_nSeatID);
		response.WriteInt64(player->m_lMoney);
		response.WriteByte(timeout);
		response.WriteString(player->json);
		Player* banker = table->getBanker();
		response.WriteInt(banker ? banker->id : 0);
		response.WriteShort(banker ? banker->m_nSeatID : -1);
		response.WriteInt64(banker ? banker->m_lMoney : 0);
		response.WriteString(banker ? banker->name : "");
		response.WriteString(banker ? banker->json : "");
		response.WriteByte((BYTE)(table->m_nMaxBankerNum - table->bankernum));
		response.WriteInt(table->m_nLimitCoin);
		response.WriteInt(table->m_nMinBetNum);
		response.WriteInt(table->m_nMaxBetNum);
		response.WriteInt64(table->m_nBankerlimit);		// 上庄限制
	}
	response.End();

	if (HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
	{
		ULOGGER(E_LOG_ERROR, player->id) << "send to hall failed, hall id = " << player->m_nHallid;
	}
	else
	{
		ULOGGER(E_LOG_ERROR, player->id) << "send to hall success, hall id = " << player->m_nHallid;
	}

	return 0;
}

