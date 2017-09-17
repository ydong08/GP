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

int IProcess::sendTableStatus(Table* table, short ntime)
{
	if(table == NULL)
		return -1;
	
	int sendnum = 0;
	int i = 0;

	CoinConf::getInstance()->setTableStatus(table->m_nStatus);

	if(table->m_nStatus == STATUS_TABLE_IDLE)
	{
		table->HandleBankeList();

		//处理用户掉线的情况
		for(i = 0; i < GAME_PLAYER; ++i)
		{
			Player* player = table->player_array[i];
			if(player)
			{
				bool bleave = false;
				if((!player->isonline)&&(player->notBetCoin() && table->bankeruid != player->id))
					bleave = true;
				//处理用户半天不下注，直接踢出服务器
				if(player->source != E_MSG_SOURCE_ROBOT && player->id != table->bankeruid && !player->isbankerlist &&
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

		Player* banker = NULL;
		if(table->bankersid >=0)
			banker = table->player_array[table->bankersid];
		if(banker)
		{
			if((banker->m_lMoney < table->m_nBankerlimit) || (table->bankernum >= table->m_nMaxBankerNum) || (banker->isCanlcebanker))
			{
				banker->isCanlcebanker = false;
				table->rotateBanker();
			}
		}
	}

	sendnum = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
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
			sendnum++;
		}
	}
	//_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	//_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	//_LOG_DEBUG_("[Data Response] ntime=[%d]\n", ntime);
	
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
	int sendnum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
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
			sendnum++;
		}
	}
	//_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	//_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	//_LOG_DEBUG_("[Data Response] banker=[%d]\n", banker->id);
	//_LOG_DEBUG_("[Data Response] bankersid=[%d]\n", banker->m_nSeatID);
	return 0;
}

int IProcess::rotateBankerInfo(Table* table, Player* banker, Player* prebanker)
{
	if (table== NULL)
	{
		return -1;
	}

	//_LOG_DEBUG_("<==[rotateBankerInfo] Push [0x%04x]\n", GMSERVER_MSG_ROTATEBANKE);
	//_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int sendnum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
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
			response.WriteByte(player->isbankerlist ? 1: 0);
			response.End();
			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
			sendnum++;
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] prebanker=[%d]\n", prebanker ? prebanker->id : 0);
	_LOG_DEBUG_("[Data Response] prebankersid=[%d]\n", prebanker ? prebanker->m_nSeatID : -1);
	_LOG_DEBUG_("[Data Response] banker=[%d]\n", banker ? banker->id : 0);
	_LOG_DEBUG_("[Data Response] bankersid=[%d]\n", banker ? banker->m_nSeatID : -1);
	_LOG_DEBUG_("[Data Response] bankermoney=[%ld]\n", banker ? banker->m_lMoney : 0);
	_LOG_DEBUG_("[Data Response] name=[%s]\n", banker ? banker->name : "");
	_LOG_DEBUG_("[Data Response] json=[%s]\n", banker ? banker->json : "");

	CoinConf::getInstance()->setTableBanker(banker ? banker->id : 0);
	/*if(banker)
	{
		char buff[128] = {0};
		int num = 5 + rand()%5;
		sprintf(buff, "【系统】:%s%s",banker->name, ErrorMsg::getInstance()->getErrMsg(num));
		AllocSvrConnect::getInstance()->trumptToUser(6,buff);
	}*/
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
	int sendnum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
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
			sendnum++;
		}
	}
	//_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	//_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	//_LOG_DEBUG_("[Data Response] leavePlayer=[%d]\n", leavePlayer->id);

	return 0;
}

int IProcess::GameStart(Table* table)
{
	if(table == NULL)
		return -1;
	return 0;
}

int IProcess::GameOver(Table* table)
{
	int i,j;
	Player* banker = NULL;
	short sendnum = 0;
	int64_t maxWinCoin = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
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
			sendnum++;
		}
	}
	sendnum = 0;

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
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
			if(table->bankersid >=0)
				banker = table->player_array[table->bankersid];
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
			response.End();
            LOGGER(E_LOG_INFO/*, player->id*/) << "win score = " << player->m_lWinScore << " BankerPoint = " 
				<< table->m_cbBankerPoint << "  PlayerPoint = " << table->m_cbPlayerPoint;
			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
			sendnum++;
		}
	}
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
		outputPacket.WriteInt(table->m_nType);  
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

