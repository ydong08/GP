#include "IProcess.h"
#include "Configure.h"
#include "BaseClientHandler.h"
#include "HallManager.h"
#include "Logger.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "Room.h"
#include "RedisLogic.h"
#include "ProtocolClientId.h"
#include "ProtocolServerId.h"
#include "Util.h"
#include "GameUtil.h"
#include "GameApp.h"
#include "MoneyAgent.h"
#include "RoundAgent.h"
#include "MySqlAgent.h"

int IProcess::sendToRobotCard(Player* player, Table* table)
{
	if(player==NULL || table == NULL)
		return 0;
	if(player->source != 30)
		return 0;
	int svid = Configure::getInstance()->m_nServerId;
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
			for(int j = 0; j < MAX_COUNT; j++)
				response.WriteByte(otherplayer->card_array[j]);
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendToRobotCard] Send To Uid[%d] Error!\n", player->id);
	return 0;
}

int IProcess::GameStart(Table* table)
{
	if(table == NULL)
		return -1;
	
	table->gameStart();
	for(int i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* player = table->player_array[i];
		
		if(player)
			sendGameStartInfo(player, table);
	}

	return 0;
}

int IProcess::sendGameStartInfo(Player* player, Table* table)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	int i = 0;
	OutputPacket response;
	response.Begin(GMSERVER_GAME_START, player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(table->m_nAnte);
	response.WriteInt(table->m_nTax);
	response.WriteInt(table->player_array[table->m_nDealerIndex]->id);
	response.WriteInt(table->player_array[table->m_nFirstIndex]->id);
	response.WriteInt64(table->m_lSumPool);
	response.WriteByte(table->m_nCountPlayer);
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		short index = (table->m_nDealerIndex+i) % table->m_bMaxNum;
		if(table->player_array[index])
		{
			response.WriteInt(table->player_array[index]->id);
			response.WriteInt64(table->player_array[index]->m_lMoney);
		}
	}
	response.WriteShort(player->optype);
	if (player->m_bCardStatus == CARD_UNKNOWN)
	{
		response.WriteInt(table->m_nRase1);
		response.WriteInt(table->m_nRase2);
		response.WriteInt(table->m_nRase3);
		response.WriteInt(table->m_nRase4);
		response.WriteInt(table->m_nMaxLimit);
	}
	else
	{
		response.WriteInt(table->m_nRase2);
		response.WriteInt(table->m_nRase3);
		response.WriteInt(table->m_nRase4);
		response.WriteInt(table->m_nMaxLimit);
		response.WriteInt(table->m_nMaxLimit * 2);
	}
	response.WriteByte(Configure::getInstance()->max_round);
	response.WriteByte(Configure::getInstance()->betcointime - 3);
	response.WriteByte(player->m_bCardStatus);
	response.WriteShort(0);
    response.WriteString("");
	response.WriteByte(player->m_nChangeNum);
	response.End();

	_LOG_DEBUG_("<==[sendGameStartInfo] Push [0x%04x] to uid=[%d]\n", GMSERVER_GAME_START, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus[%d]\n",player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", player->optype);
	_LOG_DEBUG_("[Data Response] money=[%ld]\n", player->m_lMoney);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendGameStartInfo] Send To Uid[%d] Error!\n", player->id);

	return 0;
}

int IProcess::pushForbidinfo(Table* table)
{
	if(table == NULL)
		return -1;
	if(!table->isActive())
		return -1;
	int j = 0;
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;

	for(j = 0; j < table->m_bMaxNum; ++j)
	{
		Player* player = table->player_array[j];
		if(player)
		{
			OutputPacket response;
			response.Begin(PUSH_MSG_FORBID, player->id);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(player->id);
			response.WriteShort(player->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteByte(table->m_nCountPlayer);
			for(int i = 0; i < table->m_bMaxNum; ++i)
			{
				Player* otherplayer = table->player_array[i];
				if(otherplayer)
				{
					response.WriteInt(otherplayer->id);
					response.WriteByte(otherplayer->m_nUseForbidNum);
				}
			}
			response.End();
			if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
				_LOG_ERROR_("[sendPoolinfo] Send To Uid[%d] Error!\n", player->id);
		}
	}
	

	_LOG_DEBUG_("<==[pushForbidinfo] Push [0x%04x] \n", PUSH_MSG_FORBID);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);

	return 0;
}

int IProcess::RecordWinLoser(int winUid, vector<int>& losers, vector<int64_t>& loseCoin, int loserCount, Table* table)
{
    for (int i = 0; i < loserCount; ++i)
    {
        OutputPacket outputPacket;
        outputPacket.Begin(S2S_MYSQL_LOG_WIN_LOSE);
        outputPacket.WriteShort(Configure::getInstance()->m_nLevel);
        outputPacket.WriteInt(table->id);
        outputPacket.WriteInt(table->getStartTime());
        outputPacket.WriteInt(table->getEndTime());
        outputPacket.WriteShort(Configure::getInstance()->m_nServerId);
        outputPacket.WriteInt(GAME_ID);
        outputPacket.WriteInt(winUid);
        outputPacket.WriteInt(losers[i]);
        outputPacket.WriteInt64(loseCoin[i]);
        outputPacket.End();

        if (MySqlServer()->Send(&outputPacket) != 0)
        {
            _LOG_ERROR_("Send record win and loser info request to mysql server Error\n");
            return -1;
        }
    }

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
	//int sendNum = 0;

    Json::Value game_play_anaysis;
    game_play_anaysis["rlevel"] = Configure::getInstance()->m_nLevel;
    game_play_anaysis["tid"] = table->id;
    game_play_anaysis["starttime"] = (int)table->getStartTime();
    game_play_anaysis["endtime"] = (int)table->getEndTime();
    game_play_anaysis["svid"] = Configure::getInstance()->m_nServerId;

    int winUid = 0;
    vector<int> losersUid;
    vector<int64_t> losersCoin;

    std::string endtime = Util::formatGMTTimeS();
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* player = table->player_array[i];
		if(player && player->isGameOver())
		{
			if (player->m_lFinallGetCoin > 0)
			{
                if (winUid > 0)
                {
                    LOGGER(E_LOG_ERROR) << "Too more winer , " << winUid << " and " << player->id << " all are winer";
                }
                else
                {
                    winUid = player->id;
                    //LOGGER(E_LOG_DEBUG) << " winer  " << winUid ;
                }

				table->m_nPreWinIndex = player->m_nTabIndex;
				player->m_nWin++;
                ULOGGER(E_LOG_DEBUG, player->id) << "player->m_lFinallGetCoin:" << player->m_lFinallGetCoin;
				GameUtil::CalcSysWinMoney(player->m_lFinallGetCoin, player->m_nTax, table->m_nTax);
                ULOGGER(E_LOG_DEBUG, player->id) << " player->m_nTax:" << player->m_nTax << " table->m_nTax:" << table->m_nTax;
				if (!RedisLogic::UpdateTaxRank(Tax(), player->pid, Configure::getInstance()->m_nServerId, GAME_ID, Configure::getInstance()->m_nLevel, player->tid, player->id, player->m_nTax))
					LOGGER(E_LOG_DEBUG) << "OperationRedis::GenerateTip Error, pid=" << player->pid << ", m_nServerId=" << Configure::getInstance()->m_nServerId << ", gameid=" << GAME_ID << ", level=" << Configure::getInstance()->m_nLevel << ", id=" << player->id << ", Tax=" << player->m_nTax;
				
			}
			else if (player->m_lFinallGetCoin < 0)
			{
				player->m_nLose++;
				player->m_nTax = 0;

                //LOGGER(E_LOG_DEBUG) << "loser :" << player->id << " , lose money: " << player->m_lFinallGetCoin;
                losersUid.push_back(player->id);
                losersCoin.push_back(-player->m_lFinallGetCoin);
			}
			player->m_lMoney += player->m_lFinallGetCoin;

            player->recordWin(player->m_lFinallGetCoin);

            if (player->isdropline)
            {
				RedisLogic::setPlayerOffline(Offline(), GAME_ID, Configure::getInstance()->m_nLevel,
					player->id, player->m_lFinallGetCoin,
					endtime);
            }

            char uidKey[128] = { 0 };
            sprintf(uidKey , "%d" , player->id);
            game_play_anaysis[uidKey] = player->gameRecordAnalysisInfo();
            player->clearRecordAnalysisInfo();

            char statistic_info[128] = { 0 };
            sprintf(statistic_info , ", money=%ld, FinallGetCoin=%ld, tax=%d" , player->m_lMoney , player->m_lFinallGetCoin , player->m_nTax);

			//ULOGGER(E_LOG_DEBUG, player->id) << "money=" << player->m_lMoney << " FinallGetCoin=" << player->m_lFinallGetCoin << " tax=" << player->m_nTax;
			
			player->calcMaxWin();
			if(player->m_lFinallGetCoin != 0 || player->hasUseCoin())
				updateDB_UserCoin(player, table);

			int64_t winmoney = player->m_lFinallGetCoin;
			std::string strTrumpt;
			if (GameUtil::getDisplayMessage(strTrumpt, GAME_ID, Configure::getInstance()->m_nLevel, player->name, winmoney, Configure::getInstance()->wincoin1,
				Configure::getInstance()->wincoin2, Configure::getInstance()->wincoin3, Configure::getInstance()->wincoin4))
			{
				AllocSvrConnect::getInstance()->trumptToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), player->pid);
			}
		}
	}

    Json::FastWriter jWriter;
    std::string analysisInfo = jWriter.write(game_play_anaysis);
    LOGGER(E_LOG_DEBUG)<< "flowergameanalysis:  "<< analysisInfo;

    RecordWinLoser(winUid, losersUid, losersCoin, losersUid.size(), table);

	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["Time"]=(int)(time(NULL) - table->getStartTime());
	data["num"] = table->m_bMaxNum;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{       
		char szplayer[16] = {0};
		sprintf(szplayer, "uid%d",i);
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{       
			sendGameOverinfo(otherplayer, table);
			data[szplayer] = otherplayer->id;
			sprintf(szplayer, "fvalue%d",i);
			data[szplayer] = otherplayer->m_nEndCardType;
			sprintf(szplayer, "fcoin%d",i);
			data[szplayer] = (double)otherplayer->m_lFinallGetCoin;
			sprintf(szplayer, "money%d",i);
			data[szplayer] = (double)otherplayer->m_lMoney;
		}
		else    
			data[szplayer] = 0;
	}
	
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
	return 0;
}


int IProcess::sendGameOverinfo(Player* player, Table* table)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	int i = 0;
	//int j = 0;

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
			response.WriteByte(otherplayer->m_bCardStatus);
			//printf("otherplayer:%d m_bCardStatus:%d\n", otherplayer->id, otherplayer->m_bCardStatus);
			if(otherplayer->id == player->id)
			{
				response.WriteByte(otherplayer->card_array[0]);
				response.WriteByte(otherplayer->card_array[1]);
				response.WriteByte(otherplayer->card_array[2]);
				response.WriteByte(otherplayer->m_nEndCardType);
			}
			else
			{
				set<int>::iterator iter = player->m_setCompare.find(otherplayer->id); 
				if(iter != player->m_setCompare.end())
				{
					response.WriteByte(otherplayer->card_array[0]);
					response.WriteByte(otherplayer->card_array[1]);
					response.WriteByte(otherplayer->card_array[2]);
					response.WriteByte(otherplayer->m_nEndCardType);
				}
				else
				{
					response.WriteByte(0);
					response.WriteByte(0);
					response.WriteByte(0);
					response.WriteByte(0);
				}
			}
			response.WriteInt64(otherplayer->m_lFinallGetCoin);
			response.WriteInt64(otherplayer->m_lMoney);
			response.WriteInt(otherplayer->m_nWin);
			response.WriteInt(otherplayer->m_nLose);
			response.WriteInt(otherplayer->m_nRoll);
		}
	}
	response.WriteByte(player->m_bCompleteTask ? 1 : 0);
	response.WriteShort(player->m_bCompleteTask ? player->m_nGetRoll : 0);
	response.End();

	_LOG_INFO_("[GameOverinfo] Push [0x%04x] to uid=[%d] m_lFinallGetCoin[%ld] m_lMoney[%ld]  CompleteTask[%d]\n", 
		GMSERVER_MSG_GAMEOVER, player->id,player->m_lFinallGetCoin, player->m_lMoney, player->m_bCompleteTask ? 1 : 0);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] m_nStatus[%d]\n",player->m_nStatus);

	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendGameOverinfo] Send To Uid[%d] Error!\n", player->id);
	return 0;
}

int IProcess::serverPushLeaveInfo(Table* table, Player* leavePlayer,short retno)
{
	if(table == NULL || leavePlayer == NULL)
		return -1;
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	
	_LOG_INFO_("<==[serverPushLeaveInfo] Push [0x%04x] leaver[%d] retno[%d]\n", SERVER_MSG_KICKOUT, leavePlayer->id, retno);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int i = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(SERVER_MSG_KICKOUT,table->player_array[i]->id);
            response.SetSource(E_MSG_SOURCE_GAME_SERVER);
            response.WriteShort(retno);
			response.WriteString(_EMSG_(retno));
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(leavePlayer->id);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
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
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	_LOG_DEBUG_("<==[serverWarnPlayerKick] Push [0x%04x]\n", GMSERVER_WARN_KICK);
// 	int sendnum = 0;
// 	int i = 0;
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
        ULOGGER(E_LOG_INFO, player->id) << "before m_nMagicCoinCount:" << player->m_nMagicCoinCount << " m_lMoney:" << player->m_lMoney << " m_nMagicNum:" << player->m_nMagicNum;
		MoneyAgent* connect = MoneyServer();
		OutputPacket respone;
		respone.Begin(CMD_MONEY_UPDATE_MONEY);
		respone.WriteInt(player->id); 
		respone.WriteByte(1);
		respone.WriteInt64(player->m_lFinallGetCoin);
		respone.End();
		if(connect->Send(&respone) < 0)
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
		if(RoundServer()->Send(&outPacket) < 0)
			_LOG_ERROR_("Send request to RoundServerConnect Error\n" );
	}

    int32_t round_end_type = GAME_END_NORMAL;
    if (player->is_discard == true)
    {
        round_end_type = GAME_END_DISCARD;
    }
    else if (player->isdropline)
    {
        round_end_type = GAME_END_DISCONNECT;
    }
    
	int cmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(cmd);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt64(player->m_lFinallGetCoin - table->m_nTax);
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(player->m_nTax); 
	outputPacket.WriteInt(table->m_nAnte);  
	outputPacket.WriteInt(Configure::getInstance()->m_nServerId);  
	outputPacket.WriteInt(table->id);  
	short m_nType = table->m_nType;
	outputPacket.WriteInt(m_nType);  
	outputPacket.WriteString(table->getGameID());  
	//outputPacket.WriteInt(table->getEndType());
    outputPacket.WriteInt(round_end_type);
	outputPacket.WriteInt64(player->m_lMoney); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(table->getEndTime());  
	outputPacket.WriteInt(player->m_nEndCardType);
// 	outputPacket.WriteInt(0);		//taskcoin
//  outputPacket.WriteInt(player->m_nMagicNum); 
//  outputPacket.WriteInt(player->m_nMagicCoinCount); 
//  outputPacket.WriteInt(player->m_nUseMulCoin); 
//  outputPacket.WriteInt(player->m_nUseForbidCoin); 
//  outputPacket.WriteInt(player->m_nUseChangeCoin); 
	outputPacket.End();


	if(MySqlServer()->Send( &outputPacket )==0)
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

int IProcess::updateDB_UserCoin(LeaverInfo* player, Table* table)
{
	if(table->isAllRobot())
		return 0;
	if(player->source != 30)
	{
		LOGGER(E_LOG_INFO) << "before m_nMagicCoinCount:" << player->m_nMagicCoinCount << " m_lMoney:" << player->m_lMoney << " m_nMagicNum:" << player->m_nMagicNum;
		if(player->m_lMoney - player->m_nMagicCoinCount < 0)
		{
			player->m_nMagicCoinCount = player->m_lMoney;
			player->m_lMoney = 0;
		}
        OutputPacket respone;
		respone.Begin(CMD_MONEY_UPDATE_MONEY);
		respone.WriteInt(player->uid); 
		respone.WriteByte(1); 
		int64_t tempmoney = -player->m_lSumBetCoin - table->m_nTax - player->m_nMagicCoinCount - player->m_nUseMulCoin - player->m_nUseForbidCoin - player->m_nUseChangeCoin;
		respone.WriteInt64(tempmoney);
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
	outputPacket.WriteInt64(-player->m_lSumBetCoin - table->m_nTax);
	outputPacket.WriteInt(player->source);
	outputPacket.WriteInt(player->cid);
	outputPacket.WriteInt(player->sid);
	outputPacket.WriteInt(player->pid);
	outputPacket.WriteInt(table->m_nTax);
	outputPacket.WriteInt(table->m_nAnte);
	outputPacket.WriteInt(Configure::getInstance()->m_nServerId);
	outputPacket.WriteInt(table->id);
	short m_nType = table->m_nType;
	outputPacket.WriteInt(m_nType);
	outputPacket.WriteString(table->getGameID());
	//outputPacket.WriteInt(table->getEndType());
    outputPacket.WriteInt(GAME_END_DISCONNECT);
	outputPacket.WriteInt64(player->m_lMoney); 
	outputPacket.WriteInt(table->getStartTime());  
	//outputPacket.WriteInt(table->getEndTime());
	outputPacket.WriteInt(time(NULL));  
	outputPacket.WriteInt(-1); 
// 	outputPacket.WriteInt(0); 
// 	outputPacket.WriteInt(player->m_nMagicNum); 
// 	outputPacket.WriteInt(player->m_nMagicCoinCount); 
// 	outputPacket.WriteInt(player->m_nUseMulCoin); 
// 	outputPacket.WriteInt(player->m_nUseForbidCoin); 
// 	outputPacket.WriteInt(player->m_nUseChangeCoin); 
	outputPacket.End();


	if(MySqlServer()->Send( &outputPacket )==0)
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

int IProcess::NotifyPlayerNetStatus(Table* table, int uid, int signal, int type)
{
    OutputPacket response;
    response.Begin(CLIENT_MSG_NETWORK_STATUS);
    response.WriteShort(0);
    response.WriteString("ok");
    response.WriteInt(uid);
    response.WriteInt(signal);
    response.WriteInt(type);
    response.End();
    for (int i = 0; i < GAME_PLAYER; ++i) //转发网络状态给同桌的其他玩家
    {
        Player* p = table->player_array[i];
        if (p && p->id != uid)
        {
            response.SetUid(p->id);
            HallManager::getInstance()->sendToHall(p->m_nHallid, &response, false);
        }
    }
	return 0;
}


int IProcess::sendCompareInfoToPlayers(Player* player , Table* table , Player* compareplayer , Player* opponent , short compareRet , Player* nextplayer , int64_t comparecoin , short seq)
{
    int svid = Configure::getInstance()->m_nServerId;
    int tid = (svid << 16) | table->id;
    OutputPacket response;
    response.Begin(CLIENT_MSG_COMPARE_CARD , player->id);
    //if (player->id == compareplayer->id)
    //    response.SetSeqNum(seq);
    response.WriteShort(0);
    response.WriteString("");
    response.WriteInt(player->id);
    response.WriteShort(player->m_nStatus);
    response.WriteInt(tid);
    response.WriteShort(table->m_nStatus);
    response.WriteByte(table->m_bCurrRound);
    response.WriteInt64(comparecoin);
    response.WriteByte(compareRet);
    response.WriteInt(compareplayer->id);
    response.WriteInt64(compareplayer->m_lSumBetCoin);
    response.WriteInt64(compareplayer->m_lMoney);
    response.WriteInt(opponent->id);
    response.WriteInt64(table->m_lCurrBetMax);
    response.WriteInt(nextplayer ? nextplayer->id : 0);
    response.WriteInt64(player->m_lSumBetCoin);
    response.WriteInt64(player->m_lMoney);
    response.WriteShort(player->optype);
    response.WriteInt64(table->m_lSumPool);
    response.WriteInt64(player->m_AllinCoin);
    response.WriteByte(player->m_bCardStatus);
    response.End();

    _LOG_DEBUG_("<==[CompareCardProc] Push [0x%04x] to uid=[%d]"
        "[Data Response] err=[0], errmsg[], uid=[%d],m_nStatus=[%d],m_lCurrBetMax=[%d], compareplayer=[%d],"
        "m_bCurrRound=[%d], compareRet=[%d],m_lSumBetCoin=[%d], m_lSumPool=[%d], nextplayer=[%d]\n" ,
        CLIENT_MSG_COMPARE_CARD ,
        player->id ,
        player->id ,
        player->m_nStatus ,
        table->m_lCurrBetMax ,
        compareplayer->id ,
        table->m_bCurrRound ,
        compareRet ,
        player->m_lSumBetCoin ,
        table->m_lSumPool ,
        nextplayer ? nextplayer->id : 0);

    if (HallManager::getInstance()->sendToHall(player->m_nHallid , &response , false) < 0)
        _LOG_ERROR_("[CompareCardProc] Send To Uid[%d] Error!\n" , player->id);

    return 0;
}


// 剩余两个玩家时，一个玩家只够跟注一次时，在玩家跟注时，强制进行比牌
int IProcess::ForceCampareCard(Table* table , Player* p1 , Player* p2, short seq, bool isDelayNotify)
{
    if (!table || !p1 || !p2)
    {
        _LOG_ERROR_("table:%d, campare card, peoples ptr(p1:%d, p2:%d) Error!\n" , table, p1, p2);
        return -1;
    }

    short compareRet = table->compareCard(p1 , p2);
    if (compareRet < 0)
    {
        _LOG_ERROR_("Compare Card Error, players[%d, %d], tid:%d !\n" , p1->id , p2->id, table->id );
        return -1;
    }

    // 玩家的钱全部扣除 用于比牌
    int64_t compareMoney = p1->m_lMoney;
    table->playerBetCoin(p1, compareMoney);

    // 记录比牌到游戏回放日志中
    p1->recordCompareCard(table->m_bCurrRound , p2->id , compareMoney , compareRet);

    Json::Value data;
    data["BID"] = string(table->getGameID());
    data["Time"] = (int)(time(NULL) - table->getStartTime());
    data["currd"] = table->m_bCurrRound;
    data["UID"] = p1->id;
    data["ccoin"] = (double)compareMoney;
    data["opponet"] = p2->id;
    data["count"] = (double)p1->m_lSumBetCoin;
    if (!table->isAllRobot())
        _LOG_REPORT_(player->id , RECORD_COMPARE_CARD , "%s" , data.toStyledString().c_str());
    data["errcode"] = 0;

    if (isDelayNotify && table->setCompareResultInfo(compareRet , p1 , p2, compareMoney))  // 保存比牌结果，并延迟3秒通知客户端
    {
        table->startCompareResultNotifyTimer(COMPARE_NOTIFY_DELAY_TIME);
        return 0;
    }

    p1->m_bCompare = true;
    if (compareRet == 1)
    {
        p2->m_bCardStatus = CARD_DISCARD;
    }

    if (compareRet == 0 || compareRet == DRAW)
    {
        p1->m_bCardStatus = CARD_DISCARD;
        compareRet = 0;
    }

    for (int i = 0; i < table->m_bMaxNum; ++i)
    {
        if (table->player_array[i])
        {
            _LOG_ERROR_("Send compare card result %d of %d and %d to player %d:" , compareRet , p1->id , p2->id , table->player_array[i]->id);
            sendCompareInfoToPlayers(table->player_array[i] , table , p1 , p2 , compareRet , NULL , compareMoney , seq);
        }
    }

    return table->gameOver(true);
}



