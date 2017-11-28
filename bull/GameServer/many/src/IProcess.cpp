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
	_LOG_DEBUG_("<==[sendTableStatus] Push [0x%04x]\n", GMSERVER_MSG_TABSTATUS);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
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
				if(!player->isonline)
					bleave = true;
				//处理用户半天不下注，直接踢出服务器
				if(player->source != 30 && player->id != table->bankeruid && !player->isbankerlist && 
					(time(NULL) - player->m_nLastBetTime) > Configure::getInstance()->kicktime)
				{
					_LOG_INFO_("sendTableStatus uid:%d has over kicktime:%d not active\n", player->id, Configure::getInstance()->kicktime);
					bleave = true;
				}

				if(bleave)
				{
					serverPushLeaveInfo(table,player,2);
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
			if((banker->m_lMoney < CoinConf::getInstance()->getCoinCfg()->bankerlimit) || (table->bankernum >= CoinConf::getInstance()->getCoinCfg()->bankernumplayer) || (banker->isCanlcebanker))
			{
				banker->isCanlcebanker = false;
				table->rotateBanker();
			}

			table->setBetLimit(banker);
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
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] ntime=[%d]\n", ntime);
	
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
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] banker=[%d]\n", banker->id);
	_LOG_DEBUG_("[Data Response] bankersid=[%d]\n", banker->m_nSeatID);
	return 0;
}

int IProcess::rotateBankerInfo(Table* table, Player* banker, Player* prebanker)
{
	if (table== NULL)
	{
		return -1;
	}

	_LOG_DEBUG_("<==[rotateBankerInfo] Push [0x%04x]\n", GMSERVER_MSG_ROTATEBANKE);
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
			response.WriteByte(CoinConf::getInstance()->getCoinCfg()->bankernumplayer);
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
	
	_LOG_DEBUG_("<==[serverPushLeaveInfo] Push [0x%04x]\n", SERVER_MSG_KICKOUT);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int sendnum = 0;
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(SERVER_MSG_KICKOUT,table->player_array[i]->id);
			response.WriteShort(0);
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
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] leavePlayer=[%d]\n", leavePlayer->id);

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
    std::string endtime = Util::formatGMTTimeS();

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
		Player* player = table->player_array[i];
		
		if(player)
		{
			if(player->m_lWinScore != 0)
			{
				if(player->m_lWinScore > 0)
					player->m_nWin++;

				if(player->m_lWinScore < 0)
					player->m_nLose++;
				updateDB_UserCoin(player, table);
			}

            if (!player->isonline  && player->source != 30)
            {
				RedisLogic::setPlayerOffline(Offline(), player->id, GAME_ID, Configure::getInstance()->m_nLevel,
                                                          player->m_lWinScore, endtime);
            }

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
			for(int m=0; m < 5; ++m)
			{
				for(int n = 0; n < 5; ++n)
				{
					response.WriteByte(table->m_bTableCardArray[m][n]);
				}
				response.WriteByte(table->m_CardResult[m].type);
				response.WriteByte(table->m_CardResult[m].mul);
				if(m > 0)
				{
					int index = table->m_nRecordLast -1;
					index = index < 0 ? MAX_SCORE_HISTORY -1 : index;
					response.WriteInt64(player->m_lResultArray[m]);
					if(m == 1)
					{
						response.WriteByte(table->m_GameRecordArrary[index].cbTian);
						//_LOG_DEBUG_("tian:%d\n",table->m_GameRecordArrary[index].cbTian);
					}
					if(m == 2)
					{
						response.WriteByte(table->m_GameRecordArrary[index].cbDi);
						//_LOG_DEBUG_("di:%d\n",table->m_GameRecordArrary[index].cbDi);
					}
					if(m == 3)
					{
						response.WriteByte(table->m_GameRecordArrary[index].cbXuan);
						//_LOG_DEBUG_("xuan:%d\n",table->m_GameRecordArrary[index].cbXuan);
					}
					if(m == 4)
					{
						response.WriteByte(table->m_GameRecordArrary[index].cbHuang);
						//_LOG_DEBUG_("huang:%d\n",table->m_GameRecordArrary[index].cbHuang);
					}
				}
			}
			response.WriteInt64(player->m_lWinScore);
			response.WriteInt64(player->m_lReturnScore);
			response.WriteInt64(player->m_lMoney);
			response.WriteByte(CoinConf::getInstance()->getCoinCfg()->bankernumplayer - table->bankernum);
			response.WriteInt64(table->m_lBankerWinScoreCount);
			if(table->bankersid >=0)
				banker = table->player_array[table->bankersid];
			response.WriteInt64(banker ? banker->m_lMoney : 0);
			response.End();
			_LOG_INFO_("[GameOver] GameID[%s] uid[%d] sid[%d] win[%ld] return[%ld] tian[%ld] di[%ld] xuan[%ld] huang[%ld] Money[%ld] tax[%ld]\n", table->getGameID(),player->id, player->m_nSeatID, player->m_lWinScore, 
				player->m_lReturnScore, player->m_lBetArray[1], player->m_lBetArray[2], player->m_lBetArray[3], player->m_lBetArray[4], player->m_lMoney, player->tax);
			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
			sendnum++;
		}
	}

	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["Time"]= (int)table->getEndTime();
	data["banker"] = table->bankeruid;
	data["svid"] = (int)Configure::getInstance()->m_nServerId;
	data["bet1"] = (double)table->m_lTabBetArray[1];
	data["bet2"] = (double)table->m_lTabBetArray[2];
	data["bet3"] = (double)table->m_lTabBetArray[3];
	data["bet4"] = (double)table->m_lTabBetArray[4];
	data["rbet1"] = (double)table->m_lRealBetArray[1];
	data["rbet2"] = (double)table->m_lRealBetArray[2];
	data["rbet3"] = (double)table->m_lRealBetArray[3];
	data["rbet4"] = (double)table->m_lRealBetArray[4];
	for(i = 0; i < 5; i++)
	{
		char cardbuff[64]={0};
		for(j = 0; j < 5; ++j)
		{
			sprintf(cardbuff+5*j, "0x%02x ",table->m_bTableCardArray[i][j]);
		}
		char szkey[16] = {0};
		sprintf(szkey,"card%d",i);
		data[szkey] = string(cardbuff);
	}

	if((banker && banker->source != 30) || !table->isAllRobot())
		_LOG_REPORT_(table->bankeruid, RECORD_GAME_OVER, "%s", data.toStyledString().c_str());

	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] bankeruid=[%d]\n", table->bankeruid);
	_LOG_DEBUG_("[Data Response] bankernum=[%d]\n", CoinConf::getInstance()->getCoinCfg()->bankernumplayer - table->bankernum);
	_LOG_DEBUG_("[Data Response] lBankerWinScoreCount=[%ld]\n", table->m_lBankerWinScoreCount);
	_LOG_DEBUG_("[Data Response] bankerMoney=[%ld]\n", banker ? banker->m_lMoney:0);
	return 0;
}

int IProcess::serverWarnPlayerKick(Table* table, Player* warnner, short timeCount)
{
	if(table == NULL || warnner == NULL)
		return -1;
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	
	_LOG_DEBUG_("<==[serverWarnPlayerKick] Push [0x%04x]\n", GMSERVER_WARN_KICK);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	//int sendnum = 0;
	//int i = 0;
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
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] warnner=[%d]\n", warnner->id);
	_LOG_DEBUG_("[Data Response] timeCount=[%d]\n", timeCount);

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
		respone.WriteInt64(player->m_lWinScore);
		respone.End();
		if(MoneyServer()->Send(&respone) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );

		OutputPacket outPacket;
		outPacket.Begin(ROUND_SET_INFO);
		outPacket.WriteInt(player->id); 
		outPacket.WriteInt(player->m_nWin );
		outPacket.WriteInt(player->m_nLose);
		outPacket.WriteInt(player->m_nTie);
		outPacket.WriteInt(player->m_nRunAway);
		outPacket.End();
		if(RoundServer()->Send( &outPacket ) < 0)
			_LOG_ERROR_("Send request to RoundServerConnect Error\n" );
	}


	int cmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(cmd);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt64(player->m_lWinScore);
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(player->tax); //table->m_nTax
	outputPacket.WriteInt(0);			//table->m_nAnte;
	outputPacket.WriteInt(Configure::getInstance()->m_nServerId);
	int tid = table->id | 0xFFFF;
	outputPacket.WriteInt(tid);
	outputPacket.WriteInt(table->m_nType);  
	outputPacket.WriteString(table->getGameID());  
	int endtype = table->m_CardResult[1].type<<24|table->m_CardResult[2].type<<16|table->m_CardResult[3].type<<8|table->m_CardResult[4].type;
	//outputPacket.WriteInt(endtype);
    outputPacket.WriteInt(player->isonline?0:1);
	outputPacket.WriteInt64(player->m_lMoney); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(table->getEndTime());
	outputPacket.WriteInt(table->m_CardResult[0].type);
	outputPacket.End();
	if(MySqlServer()->Send( &outputPacket ) >=0)
	{
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
	OutputPacket outputPacket;
	outputPacket.Begin(ROUND_SET_ONLINETIME);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt(time(NULL) - player->getEnterTime());
	outputPacket.End();	
 
	_LOG_INFO_("==>[updateDB] [0x%04x] [UpdateDBActiveTime]\n", ROUND_SET_ONLINETIME);
 
	if(RoundServer()->Send( &outputPacket ) >= 0)
	{
		_LOG_DEBUG_("Transfer request to RoundServerConnect OK\n" );
		return 0;
	}
	else
	{
		 _LOG_ERROR_("Send request to RoundServerConnect Error\n" );
		 return -1;
	}
	return 0;
}

int IProcess::NotifyRobot(Table * table, Player * banker, Player* vip, uint8_t bettype, int64_t betmoney)
{
	int sendnum = 0;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		if (sendnum == table->m_nCountPlayer)
			break;
		Player* player = table->player_array[i];
		if (player && player->source == E_MSG_SOURCE_ROBOT) //确保只给机器人发出通知
		{
			OutputPacket response;
			response.Begin(GMSREVER_MSG_NOTIFY_ROBOT, player->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(player->id);
			response.WriteInt(vip->id);
			for (int j = 1; j < BETNUM; j++)
			{
				response.WriteShort(vip->continuedPlayCount[j] + 1);	// note：一旦有下注，至少一局
				response.WriteInt64(vip->m_lBetArray[j]);
			}
			response.WriteInt(banker->id);
			response.WriteInt64(banker->m_lMoney);
			response.End();
			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
		}
		sendnum++;
	}
	return 0;
}

int IProcess::SaveHundredDiscount(Table* table, int discount)
{
	OutputPacket outputPacket;
	outputPacket.Begin(CMD_MYSQLSVR_HUNDRED_DISCOUNT);
	outputPacket.SetOptType(GAME_ID);
	outputPacket.WriteInt(Configure::getInstance()->m_nLevel);
	outputPacket.WriteInt(Configure::getInstance()->m_nServerId);
	int tid = table->id | 0xFFFF;
	outputPacket.WriteInt(tid);
	outputPacket.WriteInt(discount);
	outputPacket.WriteInt(table->getStartTime());
	outputPacket.WriteInt(table->getEndTime());
	outputPacket.End();
	if(MySqlServer()->Send( &outputPacket ) >= 0)
	{
		return 0;
	}
	_LOG_ERROR_("Send request to BackServer Error\n" );
	return -1;
}
