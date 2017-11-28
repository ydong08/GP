#include "IProcess.h"
#include "Configure.h"
#include "ClientHandler.h"
#include "HallManager.h"
#include "Logger.h"
#include "MySqlConnect.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "MoneyConnect.h"
#include "RoundServerConnect.h"
#include "TaskRedis.h"
#include "LogServerConnect.h"
#include "UdpManager.h"
#include "Util.h"
#include "ProtocolClientId.h"
#include "OfflineRedis.h"
#include "BasePlayer.h"

int IProcess::send(CDLSocketHandler* clientHandler, OutputPacket* outPacket, bool isEncrypt)
{
	if(clientHandler && outPacket)
	{
		if(isEncrypt)
			outPacket->EncryptBuffer();
		return clientHandler->Send((const char*)outPacket->packet_buf(), outPacket->packet_size()) ;
	}
	return -1;	 
}

int IProcess::sendErrorMsg(ClientHandler* clientHandler,int cmd, int uid , short errcode, const char* errmsg, unsigned short seq)
{
	OutputPacket response;
	response.Begin(cmd,uid);
	response.SetSeqNum(seq);
	response.WriteShort(errcode);
	response.WriteString(errmsg);
	response.WriteInt(uid);
	response.End();
	_LOG_WARN_( "[DATA Reponse] uid=[%d] erron=[%d] errmsg=[%s] \n", uid, errcode, errmsg);
	Json::Value data;
	data["errcode"] = errcode;
	data["errmsg"] = errcode;
	_UDP_REPORT_(uid, cmd,"%s",data.toStyledString().c_str());
	if(clientHandler)
		clientHandler->Send(&response,false);
	return 0;
}

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
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteShort(table->countPlayer);
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			for(int j = 0; j < 5; j++)
				response.WriteByte(otherplayer->card_array[j]);
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[sendToRobotCard] Send To Uid[%d] Error!\n", player->id);
	return 0;
}

int IProcess::GameStart( Table* table)
{
	if(table == NULL)
		return -1;
	table->gameStart();

	int sendNum = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{	
		if(sendNum == table->countPlayer)
			break;
		Player* player = table->player_array[i];
		if(player)
		{
			if(player->isActive())
			{
				//默认让用户下一个低注，并且把台费扣除
				player->betCoinList[0] += player->carrycoin < (int64_t)table->ante ? player->carrycoin :(int64_t)table->ante;
				if(player->source == 30)
				{
					sendToRobotCard(player, table);
				}
			}
			sendGameStartInfo(player, table);
			sendNum++;
		}
	}

	table->setNextRound();

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
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteInt64(table->ante);
	response.WriteInt64(table->thisGameLimit);
	response.WriteShort(table->raterent);
	response.WriteShort(Configure::getInstance()->betcointime - 3);
	response.WriteShort(table->countPlayer);
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if (sendNum == table->countPlayer)
			break;
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteShort(otherplayer->status);
			response.WriteShort(otherplayer->tab_index);
			response.WriteInt64(otherplayer->carrycoin);
			sendNum++;
		}
	}

	if(player->task1)
	{
		response.WriteInt64(player->task1->taskid);
		response.WriteString(player->source == 1 ? player->task1->taskANDname : player->task1->taskIOSname);
		response.WriteShort(player->ningot);
	}
	else
	{
		response.WriteInt64(0);
		response.WriteString("");
		response.WriteShort(0);
	}
	response.End();

	_LOG_DEBUG_("<==[sendGameStartInfo] Push [0x%04x] to uid=[%d]\n", GMSERVER_GAME_START, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] status[%d]\n",player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] ante=[%ld]\n", table->ante);
	_LOG_DEBUG_("[Data Response] thisGameLimit=[%ld]\n", table->thisGameLimit);
	_LOG_DEBUG_("[Data Response] raterent=[%d]\n", table->raterent);
	_LOG_DEBUG_("[Data Response] betcointime=[%d]\n", Configure::getInstance()->betcointime - 3);
	_LOG_DEBUG_("[Data Response] countPlayer=[%d]\n", table->countPlayer);
	_LOG_DEBUG_("[Data Response] carrycoin=[%d]\n", player->carrycoin);

	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[sendGameStartInfo] Send To Uid[%d] Error!\n", player->id);

	return 0;
}

int IProcess::sendRoundCard(Player* player, Table* table, Player* firstBetPlayer, Player* prewinner, short currRound)
{
	if(player == NULL || table == NULL || firstBetPlayer == NULL || prewinner == NULL)
		return -1;
	//只允许当前轮数在1 - 6 之间，非别代表发牌第一轮至第四轮，5 代表大家翻底牌决定胜负
	if(currRound < 0 || currRound > 6)
		return -1;
	/*if(!player->hascard)
	{
		_LOG_INFO_("player[%d] in table[%d] has throw out card\n", player->id, table->id);
		return 0;
	}*/

	int i = 0;
	int sendNum = 0;
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;

	OutputPacket response;
	response.Begin(GMSERVER_MSG_SEND_CARD, player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteInt(firstBetPlayer->id);
	response.WriteShort(firstBetPlayer->optype);
	response.WriteByte(player->card_array[0]);
	response.WriteShort(currRound);
	response.WriteInt64(firstBetPlayer->nextlimitcoin);
	response.WriteShort(table->countPlayer);
	short start = 0;
	//是第一轮则判断上一盘赢牌的用户是否还在，如果在就给他先发牌
	if(currRound == 1)
	{
		if(table->pregame_winner)
			start = table->pregame_winner->tab_index;
	}
	//否则都是前一个牌面大的先发牌
	else
		start= prewinner->tab_index;
	
	if(start < 0)
		start = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		short index = (start + i)%GAME_PLAYER;
		if(sendNum == table->countPlayer)
			break;
		Player* otherplayer = table->player_array[index];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteShort(otherplayer->status);
			response.WriteShort(otherplayer->tab_index);
			response.WriteInt64(otherplayer->betCoinList[0]);
			//是否已经弃牌
			if(otherplayer->hascard)
			{
				response.WriteShort(1);
				//当前还没有到所有要亮底牌的时候
				if(currRound != 6)
				{
					//如果第五轮则发牌也只是发明牌的
					if(currRound == 5)
					{
						for(int j = 1; j < 5; j++)
							response.WriteByte(otherplayer->card_array[j]);
					}
					else
					{
						for(int j = 1; j < currRound + 1; j++)
							response.WriteByte(otherplayer->card_array[j]);
					}
					//返回当前用户手牌类型
					short cardType = table->GetPlayerCardKind(otherplayer->card_array, 1, currRound == 5 ? 4 : currRound);
					response.WriteShort(cardType); 
				}
				else
				{
					//如果是第六轮直接把所有玩家的底牌都发过去
					for(int j = 0; j < 5; j++)
						response.WriteByte(otherplayer->card_array[j]);
					//返回当前用户手牌类型
					short cardType = table->GetPlayerCardKind(otherplayer->card_array, 0, 4);
					response.WriteShort(cardType); 
				}
			}
			else
				response.WriteShort(0);
			sendNum++;
		}
	}
	response.End();

	_LOG_DEBUG_("<==[sendRoundCard] Push [0x%04x] to uid=[%d]\n", GMSERVER_MSG_SEND_CARD, player->id);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] status[%d]\n",player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] currRound=[%d]\n", currRound);
	_LOG_DEBUG_("[Data Response] firstBetPlayerID=[%d]\n", firstBetPlayer->id);
	_LOG_DEBUG_("[Data Response] optype=[%d]\n", firstBetPlayer->optype);
	_LOG_DEBUG_("[Data Response] limitbetcoin=[%ld]\n", firstBetPlayer->nextlimitcoin);
	_LOG_DEBUG_("[Data Response] countPlayer=[%d]\n", table->countPlayer);

	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[sendRoundCard] Send To Uid[%d] Error!\n", player->id);

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
            HallManager::getInstance()->sendToHall(p->hallid, &response, false);
        }
    }
}

int IProcess::GameOver( Table* table ,Player* winner, bool isthrowwin)
{
	if(table == NULL || winner == NULL)
	{
		_LOG_ERROR_("[GameOver] table[%p] or winner[%p] is NULL\n", table, winner);
		return -1;
	}

    std::string endtime = Util::formatGMTTimeS();

	table->gameOver(winner, isthrowwin);
	int i = 0;
	int sendNum = 0;
	//计算用户的战绩和金币
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		Player* otherplayer = table->player_array[i];
		short nwin = 0;
		short nlose = 0;
		short nrunaway = 0;
		//说明这个用户才是去玩牌的用户
		if(otherplayer && (otherplayer->status == STATUS_PLAYER_OVER))
		{
			otherplayer->money += otherplayer->finalgetCoin;
			if (otherplayer->finalgetCoin >= 0)
			{
				otherplayer->nwin++;
				nwin = 1;
			}
			else if (otherplayer->finalgetCoin < 0)
			{
				otherplayer->nlose++;
				nlose = 1;
			}

			if(otherplayer->source != 30 && otherplayer->isroundtask)
				TaskRedis::getInstance()->setPlayerRound(table, otherplayer);

			if(otherplayer->m_bFinalComFlag == 1)
			{
				if(table->m_bRewardType == 2)
				{
					otherplayer->voucher += otherplayer->m_nComGetRoll;	
					updateDB_UserVoucher(otherplayer, table, otherplayer->m_nComGetRoll, true);
				}
				else
					otherplayer->money += otherplayer->m_nComGetCoin;
			}

			//扣除魔法表情的钱
			if(otherplayer->money > 0)
			{
				if(otherplayer->m_nMagicCoinCount > 0)
				{
					if(otherplayer->money - otherplayer->m_nMagicCoinCount < 0)
					{
						otherplayer->m_nMagicCoinCount = otherplayer->money;
						otherplayer->money = 0;
					}
					else
					{
						otherplayer->money -= otherplayer->m_nMagicCoinCount;
					}
				}
			}

            if (otherplayer->isdropline)
            {
                _LOG_ERROR_("<==[GameOver] %d IS OFFLINE, record game result of offline \n", otherplayer->id);
                OfflineRedis::getInstance()->setPlayerOffline( GAME_ID, 
                                                               Configure::getInstance()->level,
                                                               otherplayer->id, 
                                                               otherplayer->m_nComGetCoin,
                                                               endtime );
            }

			updateDB_UserCoin(otherplayer, table, nwin, nlose, nrunaway);
		}
	}

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendNum == table->countPlayer)
			break;
		Player* otherplayer = table->player_array[i];
		if (otherplayer)
		{
			int64_t winmoney = otherplayer->finalgetCoin;
			std::string strTrumpt;
			if (Util::getDisplayMessage(strTrumpt, GAME_ID, Configure::getInstance()->level, otherplayer->name, winmoney, Configure::getInstance()->wincoin1,
				Configure::getInstance()->wincoin2, Configure::getInstance()->wincoin3, Configure::getInstance()->wincoin4))
			{
				AllocSvrConnect::getInstance()->trumptComTasToUser(PRODUCT_TRUMPET, strTrumpt.c_str(), otherplayer->pid);
			}
			sendGameOverinfo(otherplayer, table, winner);
			sendNum++;
		}
	}

	Json::Value data;
	data["BID"] = string(table->getGameID());
	data["Time"]=(int)(time(NULL) - table->getStartTime());
	data["num"] = table->winnercount;
	for(i = 0; i < table->winnercount; ++i)
	{
		Player* otherplayer = table->getPlayer(table->winnerorder[i]);
		if(otherplayer)
		{
			char szplayer[16] = {0};
			sprintf(szplayer, "uid%d",i);
			data[szplayer] = otherplayer->id;
			sprintf(szplayer, "fvalue%d",i);
			data[szplayer] = otherplayer->finalcardvalue;
			sprintf(szplayer, "fcoin%d",i);
			data[szplayer] = (double)otherplayer->finalgetCoin;
			sprintf(szplayer, "money%d",i);
			data[szplayer] = (double)otherplayer->money;
		}
	}
	if(!table->isAllRobot())
		_LOG_REPORT_(table->raseUid, RECORD_GAME_OVER, "%s", data.toStyledString().c_str());

	for(i = 0; i < GAME_PLAYER; ++i)
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
	table->reSetInfo();
	return 0;
}


int IProcess::sendGameOverinfo(Player* player, Table* table, Player* winner)
{
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	int i = 0;

	OutputPacket response;
	response.Begin(GMSERVER_MSG_GAMEOVER,player->id);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(player->id);
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteInt(winner->id);
	response.WriteShort(table->leavercount);
	for(int j = 0; j < table->leavercount; ++j)
	{
		response.WriteInt(table->leaverarry[j].uid);
		response.WriteShort(table->leaverarry[j].tab_index);
		response.WriteString(table->leaverarry[j].name);
		response.WriteInt64(table->leaverarry[j].betcoin);
	}
	response.WriteShort(table->winnercount);
	for(i = 0; i < table->winnercount; ++i)
	{
		Player* otherplayer = table->getPlayer(table->winnerorder[i]);
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteShort(otherplayer->status);
			response.WriteShort(otherplayer->tab_index);
			response.WriteShort(otherplayer->finalcardvalue);
			response.WriteInt64(otherplayer->finalgetCoin);
			response.WriteInt64(otherplayer->money);
			response.WriteInt(otherplayer->nwin);
			response.WriteInt(otherplayer->nlose);
		}
	}

	if(player->status == STATUS_PLAYER_OVER)
	{
		bool btask1 = TaskManager::getInstance()->calcPlayerComplete(player->task1, player, table);
		if(btask1)
		{
			/*if(TaskManager::getInstance()->setPlayerComplete(player) < 0)
			{
				_LOG_ERROR_("UID=[%d] Complete taskid=[%ld] getingot=[%d] not insert redis\n", player->id, player->task1->taskid, player->ningot);
			}*/
			player->voucher += player->ningot;
			_LOG_INFO_("UID=[%d] Complete this taskid=[%ld] getingot=[%d] level[%d]\n", player->id, player->task1->taskid, player->ningot, table->clevel);
			response.WriteShort(1);
			response.WriteInt64(player->task1->taskid);
			response.WriteString(player->source == 1 ? player->task1->taskANDname : player->task1->taskIOSname);
			response.WriteShort(player->ningot);
			updateDB_UserVoucher(player, table, player->ningot);

// 			char szbuff[128] = {0};
// 			if((Configure::getInstance()->level == 1) && (player->ningot >= Configure::getInstance()->getIngotNoti1))
// 			{
// 				sprintf(szbuff, "系统消息：越高场次越多乐券，恭喜%s在%s获得%d乐券！",player->name, ErrorMsg::getInstance()->getErrMsg(3+Configure::getInstance()->level), player->ningot);				
// 				AllocSvrConnect::getInstance()->trumptComTasToUser(5, szbuff);
// 			}
// 
// 			if((Configure::getInstance()->level == 2) && (player->ningot >= Configure::getInstance()->getIngotNoti2))
// 			{
// 				sprintf(szbuff, "系统消息：恭喜%s在%s轻轻松松击败对手赢得%d乐券！",player->name, ErrorMsg::getInstance()->getErrMsg(3+Configure::getInstance()->level), player->ningot);
// 				AllocSvrConnect::getInstance()->trumptComTasToUser(5, szbuff);
// 			}
// 
// 			if((Configure::getInstance()->level == 3 ) && (player->ningot >= Configure::getInstance()->getIngotNoti3))
// 			{
// 				sprintf(szbuff, "系统消息：恭喜%s在%s轻轻松松击败对手赢得%d乐券！",player->name, ErrorMsg::getInstance()->getErrMsg(3+Configure::getInstance()->level), player->ningot);
// 				AllocSvrConnect::getInstance()->trumptComTasToUser(5, szbuff);
// 			}
		}
		else
		{
			response.WriteShort(0);
			response.WriteInt64(0);
			response.WriteString("");
			response.WriteShort(0);
		}

		if(player->istask && !player->isroundtask)
		{
			int ntask = 0;
			int ret = TaskManager::getInstance()->setNewTask(player, ntask);
			if(ntask != 0)
			{
				player->boardTask = ntask;
			}
			ntask = player->boardTask;
			int nlow = ntask & 0x00FF;
			int nmid = ntask>>8 & 0x00FF;
			int nhigh = ntask>>16 & 0x00FF;
			int ncomplete = ntask >> 24;
			//
			if(ret < 0)
			{
				_LOG_ERROR_("player[%d] set board Task Error!\n", player->id);
				response.WriteShort(0);
				response.WriteShort(0);
				response.WriteShort(0);
				response.WriteString("");
				response.WriteShort(0);
				response.WriteShort(0);
				response.WriteShort(0);
			}
			//
			if(ret == 0)
			{
				_LOG_INFO_("player[%d] set board Task Success\n", player->id);
				response.WriteShort(0);
				response.WriteShort(0);
				if(Configure::getInstance()->level == 1)
				{
					if(ncomplete & 1)
					{
						if(ncomplete & 2)
						{
							if(ncomplete & 4)
							{
								response.WriteShort(0);
								response.WriteString("");
								response.WriteShort(0);
								response.WriteShort(0);
								response.WriteShort(0);
							}
							else
							{
								response.WriteShort(1);
								response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
								response.WriteShort(nhigh);
								response.WriteShort(player->coincfg.playCount3);
							}
						}
						else
						{
							response.WriteShort(1);
							response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
							response.WriteShort(nmid);
							response.WriteShort(player->coincfg.playCount2);
							response.WriteShort(player->coincfg.getingot2);
						}
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(4));
						response.WriteShort(nlow);
						response.WriteShort(player->coincfg.playCount1);
						response.WriteShort(player->coincfg.getingot1);
					}
				}
				if(Configure::getInstance()->level == 2)
				{
					if(ncomplete & 2)
					{
						if(ncomplete & 4)
						{
							response.WriteShort(0);
							response.WriteString("");
							response.WriteShort(0);
							response.WriteShort(0);
							response.WriteShort(0);
						}
						else
						{
							response.WriteShort(1);
							response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
							response.WriteShort(nhigh);
							response.WriteShort(player->coincfg.playCount3);
							response.WriteShort(player->coincfg.getingot3);
						}
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
						response.WriteShort(nmid);
						response.WriteShort(player->coincfg.playCount2);
						response.WriteShort(player->coincfg.getingot2);
					}
				}
				if(Configure::getInstance()->level == 3)
				{
					if(ncomplete & 4)
					{
						response.WriteShort(0);
						response.WriteString("");
						response.WriteShort(0);
						response.WriteShort(0);
						response.WriteShort(0);
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
						response.WriteShort(nhigh);
						response.WriteShort(player->coincfg.playCount3);
						response.WriteShort(player->coincfg.getingot3);
					}
				}
				if(Configure::getInstance()->level > 3)
				{
					response.WriteShort(0);
					response.WriteString("");
					response.WriteShort(0);
					response.WriteShort(0);
					response.WriteShort(0);
				}
			}
			//
			if(ret == 1)
			{
				player->voucher += player->coincfg.getingot1;
				updateDB_UserVoucher(player, table, player->coincfg.getingot1, true);
				_LOG_INFO_("UID=[%d] Complete this boardtask getingot=[%d] level[%d]\n", player->id, player->coincfg.getingot1, table->clevel);
				response.WriteShort(1);
				response.WriteShort(player->coincfg.getingot1);
				if(ncomplete & 2)
				{
					if(ncomplete & 4)
					{
						response.WriteShort(0);
						response.WriteString("");
						response.WriteShort(0);
						response.WriteShort(0);
						response.WriteShort(0);
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
						response.WriteShort(nhigh);
						response.WriteShort(player->coincfg.playCount3);
						response.WriteShort(player->coincfg.getingot3);
					}
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
					response.WriteShort(nmid);
					response.WriteShort(player->coincfg.playCount2);
					response.WriteShort(player->coincfg.getingot2);
				}
			}
			//
			if(ret == 2)
			{
				player->voucher += player->coincfg.getingot2;
				updateDB_UserVoucher(player, table, player->coincfg.getingot2, true);
				_LOG_INFO_("UID=[%d] Complete this boardtask getingot=[%d] level[%d]\n", player->id, player->coincfg.getingot2, table->clevel);
				response.WriteShort(1);
				response.WriteShort(player->coincfg.getingot2);
				if(ncomplete & 4)
				{
					response.WriteShort(0);
					response.WriteString("");
					response.WriteShort(0);
					response.WriteShort(0);
					response.WriteShort(0);
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
					response.WriteShort(nhigh);
					response.WriteShort(player->coincfg.playCount3);
					response.WriteShort(player->coincfg.getingot3);
				}
			}
			//
			if(ret == 3)
			{
				player->voucher += player->coincfg.getingot3;
				updateDB_UserVoucher(player, table, player->coincfg.getingot3, true);
				_LOG_INFO_("UID=[%d] Complete this boardtask getingot=[%d] level[%d]\n", player->id, player->coincfg.getingot3, table->clevel);
				response.WriteShort(1);
				response.WriteShort(player->coincfg.getingot3);
				response.WriteShort(0);
				response.WriteString("");
				response.WriteShort(0);
				response.WriteShort(0);
				response.WriteShort(0);
			}
			response.WriteInt(player->voucher);
		}
		else if (player->isroundtask)
		{
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteInt(player->voucher);
			response.WriteByte(player->m_bFinalComFlag);
			response.WriteByte(table->m_bRewardType);
			if(table->m_bRewardType == 2)
				response.WriteInt(player->m_nComGetRoll);
			else
				response.WriteInt(player->m_nComGetCoin);

		}
	}
	//如果状态不是结束状态说明此用户只是围观，所以还是要给他传局数任务和牌型任务
	else
	{
		if(player->istask && !player->isroundtask)
		{	
			response.WriteShort(0);
			response.WriteInt64(0);
			response.WriteString("");
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteShort(0);
			int ntask = player->boardTask;
			int nlow = ntask & 0x00FF;
			int nmid = ntask>>8 & 0x00FF;
			int nhigh = ntask>>16 & 0x00FF;
			int ncomplete = ntask >> 24;
			_LOG_DEBUG_("player[%d] boardTask[%d]\n", player->id, player->boardTask);
			if(Configure::getInstance()->level == 1)
			{
				if(ncomplete & 1)
				{
					if(ncomplete & 2)
					{
						if(ncomplete & 4)
						{
							response.WriteShort(0);
							response.WriteString("");
							response.WriteShort(0);
							response.WriteShort(0);
							response.WriteShort(0);
						}
						else
						{
							response.WriteShort(1);
							response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
							response.WriteShort(nhigh);
							response.WriteShort(player->coincfg.playCount3);
							response.WriteShort(player->coincfg.getingot3);
							
						}
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
						response.WriteShort(nmid);
						response.WriteShort(player->coincfg.playCount2);
						response.WriteShort(player->coincfg.getingot2);
						
					}
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(4));
					response.WriteShort(nlow);
					response.WriteShort(player->coincfg.playCount1);
					response.WriteShort(player->coincfg.getingot1);
				}
			}
			if(Configure::getInstance()->level == 2)
			{
				if(ncomplete & 2)
				{
					if(ncomplete & 4)
					{
						response.WriteShort(0);
						response.WriteString("");
						response.WriteShort(0);
						response.WriteShort(0);
						response.WriteShort(0);
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
						response.WriteShort(nhigh);
						response.WriteShort(player->coincfg.playCount3);
						response.WriteShort(player->coincfg.getingot3);
					}
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
					response.WriteShort(nmid);
					response.WriteShort(player->coincfg.playCount2);
					response.WriteShort(player->coincfg.getingot2);
				}
			}

			if(Configure::getInstance()->level == 3)
			{
				if(ncomplete & 4)
				{
					response.WriteShort(0);
					response.WriteString("");
					response.WriteShort(0);
					response.WriteShort(0);
					response.WriteShort(0);
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
					response.WriteShort(nhigh);
					response.WriteShort(player->coincfg.playCount3);
					response.WriteShort(player->coincfg.getingot3);
				}
			}

			if(Configure::getInstance()->level > 3)
			{
				response.WriteShort(0);
				response.WriteString("");
				response.WriteShort(0);
				response.WriteShort(0);
				response.WriteShort(0);
			}
			response.WriteInt(player->voucher);
		}
		else if (player->isroundtask)
		{
			response.WriteShort(0);
			response.WriteInt64(0);
			response.WriteString("");
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteInt(player->voucher);
			response.WriteByte(player->m_bFinalComFlag);
			response.WriteByte(table->m_bRewardType);
			if(table->m_bRewardType == 2)
				response.WriteInt(player->m_nComGetRoll);
			else
				response.WriteInt(player->m_nComGetCoin);
		}
	}
	response.End();

	_LOG_INFO_("[GameOverinfo] Push [0x%04x] to uid=[%d] finalgetCoin[%ld] money[%ld] ComFlag[%d] RewardType[%d] ComGetCoin[%d] ComGetRoll[%d] MagicNum[%d] MagicCoin[%d]\n", 
		GMSERVER_MSG_GAMEOVER, player->id,player->finalgetCoin, player->money, player->m_bFinalComFlag, table->m_bRewardType, player->m_nComGetCoin, player->m_nComGetRoll, player->m_nMagicNum, player->m_nMagicCoinCount);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] uid[%d]\n",player->id);
	_LOG_DEBUG_("[Data Response] status[%d]\n",player->status);
	_LOG_DEBUG_("[Data Response] finalcardvalue[%d]\n",player->finalcardvalue);
	_LOG_DEBUG_("[Data Response] money[%d]\n",player->money);
	_LOG_DEBUG_("[Data Response] nwin[%d]\n",player->nwin);
	_LOG_DEBUG_("[Data Response] nlose[%d]\n",player->nlose);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] leavercount=[%d]\n", table->leavercount);
	_LOG_DEBUG_("[Data Response] countPlayer=[%d]\n", table->winnercount);
	_LOG_DEBUG_("[Data Response] voucher=[%d]\n", player->voucher);

	//局数任务完成则清空标志
	if(player->m_bFinalComFlag == 1)
	{
		player->m_bFinalComFlag = 0;
		player->m_nComGetCoin = 0;
		player->m_nComGetRoll = 0;
	}

	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[sendRoundCard] Send To Uid[%d] Error!\n", player->id);
	return 0;
}

int IProcess::serverPushLeaveInfo(Table* table, Player* leavePlayer,short retno)
{
	if(table == NULL || leavePlayer == NULL)
		return -1;
	int svid = Configure::getInstance()->server_id;
	int tid = (svid << 16)|table->id;
	
	_LOG_DEBUG_("<==[serverPushLeaveInfo] Push [0x%04x]\n", SERVER_MSG_KICKOUT);
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
			response.Begin(SERVER_MSG_KICKOUT,table->player_array[i]->id);
			response.WriteShort(0);
			response.WriteString(ErrorMsg::getInstance()->getErrMsg(retno));
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->status);
			response.WriteInt(tid);
			response.WriteShort(table->status);
			response.WriteInt(leavePlayer->id);
			response.WriteInt(-1);
			response.WriteShort(0);
			response.WriteInt64(-1);
			response.WriteInt64(-1);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->hallid, &response, false);
			sendnum++;
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] leavePlayer=[%d]\n", leavePlayer->id);

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
	response.WriteShort(warnner->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteShort(timeCount);
	response.End();
	HallManager::getInstance()->sendToHall(warnner->hallid, &response, false);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] warnner=[%d]\n", warnner->id);
	_LOG_DEBUG_("[Data Response] timeCount=[%d]\n", timeCount);

	return 0;
}

int IProcess::updateDB_UserCoin(Player* player, Table* table, short nwin, short nlose, short nrunaway)
{
	if(player->source != 30)
	{
		MoneyNodes* pmconnect = MoneyConnectManager::getNodes(player->id % 10 + 1);
		OutputPacket respone;
		respone.Begin(UPDATE_MONEY);
		respone.WriteInt(player->id); 
		respone.WriteByte(1);  
		if(player->m_bFinalComFlag == 1)
			respone.WriteInt64(player->finalgetCoin + player->m_nComGetCoin - player->m_nMagicCoinCount);
		else
			respone.WriteInt64(player->finalgetCoin - player->m_nMagicCoinCount);
		respone.End();
		if(pmconnect->send(&respone, false) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );

		OutputPacket outPacket;
		outPacket.Begin(ROUND_SET_INFO);
		outPacket.WriteInt(player->id); 
		outPacket.WriteInt(player->nwin);
		outPacket.WriteInt(player->nlose);
		outPacket.WriteInt(player->ntie);
		outPacket.WriteInt(player->nrunaway);
		outPacket.End();
		if(RoundServerConnect::getInstance()->Send( &outPacket ) < 0)
			_LOG_ERROR_("Send request to RoundServerConnect Error\n" );
	}

	int cmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(cmd);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt64(player->finalgetCoin);
// 	outputPacket.WriteInt(nwin);  
// 	outputPacket.WriteInt(nlose);  
// 	outputPacket.WriteInt(nrunaway);  
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(player->deducte_tax); 
	outputPacket.WriteInt(table->ante);  
	outputPacket.WriteInt(Configure::getInstance()->server_id);  
	outputPacket.WriteInt(table->id);  
	short clevel = table->clevel;
	//if(table->clevel == 6 || table->clevel == 7)
	//	clevel = 3;
	outputPacket.WriteInt(clevel);  
	outputPacket.WriteString(table->getGameID());  
	//outputPacket.WriteInt(table->getEndType());  
    outputPacket.WriteInt(player->isdropline?GAME_END_DISCONNECT:GAME_END_NORMAL);
	outputPacket.WriteInt64(player->money); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(table->getEndTime());  
	outputPacket.WriteInt(player->finalcardvalue); 
// 	if(player->m_bFinalComFlag == 1 && table->m_bRewardType == 1)
// 		outputPacket.WriteInt(player->m_nComGetCoin); 
// 	else
// 		outputPacket.WriteInt(0); 
// 	outputPacket.WriteInt(player->m_nMagicNum); 
// 	outputPacket.WriteInt(player->m_nMagicCoinCount); 
	outputPacket.End();

	_LOG_DEBUG_("==>[updateDB] [0x%04x] [updateDB_UserCoin]\n", cmd);

	MySqlConnect* connect = MySqlConnect::getInstance();
	if(connect->Send( &outputPacket )==0)
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

//处理当用户在观战的时候发送魔法表情没有扣除金币的问题
int IProcess::updateDB_UserCoin(Player* player, Table* table)
{
	if(player->source != 30)
	{
		MoneyNodes* pmconnect = MoneyConnectManager::getNodes(player->id % 10 + 1);
		OutputPacket respone;
		respone.Begin(UPDATE_MONEY);
		respone.WriteInt(player->id); 
		respone.WriteByte(1);  
		respone.WriteInt64(-player->m_nMagicCoinCount);
		respone.End();
		if(pmconnect->send(&respone, false) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );
	}

	int cmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(cmd);
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt64(0);
// 	outputPacket.WriteInt(0);  
// 	outputPacket.WriteInt(0);  
// 	outputPacket.WriteInt(0);  
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(0); 
	outputPacket.WriteInt(table->ante);  
	outputPacket.WriteInt(Configure::getInstance()->server_id);  
	outputPacket.WriteInt(table->id);  
	short clevel = table->clevel;
	outputPacket.WriteInt(clevel);  
	outputPacket.WriteString(table->getGameID());  
	//outputPacket.WriteInt(0);  
    outputPacket.WriteInt(player->isdropline?GAME_END_DISCONNECT:GAME_END_NORMAL);
	outputPacket.WriteInt64(player->money); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(time(NULL));  
	outputPacket.WriteInt(0);
// 	outputPacket.WriteInt(0); 
// 	outputPacket.WriteInt(player->m_nMagicNum); 
// 	outputPacket.WriteInt(player->m_nMagicCoinCount); 
	outputPacket.End();

	_LOG_DEBUG_("==>[updateDB] [0x%04x] [updateDB_UserCoin]\n", cmd);

	MySqlConnect* connect = MySqlConnect::getInstance();
	if(connect->Send( &outputPacket )==0)
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

int IProcess::updateDB_UserCoin(LeaverInfo* player, Table* table, short nwin, short nlose, short nrunaway)
{
	if(player->source != 30)
	{
		_LOG_DEBUG_("before m_nMagicCoinCount:%d money:%d m_nMagicNum:%d \n", player->m_nMagicCoinCount, player->money, player->m_nMagicNum);
		if(player->money - player->m_nMagicCoinCount < 0)
		{
			player->m_nMagicCoinCount = player->money;
			player->money = 0;
		}
		MoneyNodes* pmconnect = MoneyConnectManager::getNodes(player->uid % 10 + 1);
		OutputPacket respone;
		respone.Begin(UPDATE_MONEY);
		respone.WriteInt(player->uid); 
		respone.WriteByte(1); 
		respone.WriteInt64(player->betcoin - table->tax - player->m_nMagicCoinCount);
		respone.End();
		if(pmconnect->send(&respone, false) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );

		OutputPacket outPacket;
		outPacket.Begin(ROUND_SET_INFO);
		outPacket.WriteInt(player->uid); 
		outPacket.WriteInt(player->nwin + nwin);
		outPacket.WriteInt(player->nlose + nlose);
		outPacket.WriteInt(player->ntie);
		outPacket.WriteInt(player->nrunaway + nrunaway);
		outPacket.End();
		if(RoundServerConnect::getInstance()->Send( &outPacket ) < 0)
			_LOG_ERROR_("Send request to RoundServerConnect Error\n" );
		_LOG_DEBUG_("after m_nMagicCoinCount:%d money:%d m_nMagicNum:%d \n", player->m_nMagicCoinCount, player->money, player->m_nMagicNum);
	}


	int sedcmd = 0x0120;
	OutputPacket outputPacket;
	outputPacket.Begin(sedcmd);
	outputPacket.WriteInt(player->uid);  
	outputPacket.WriteInt64(player->betcoin - table->tax);
// 	outputPacket.WriteInt(nwin);  
// 	outputPacket.WriteInt(nlose);  
// 	outputPacket.WriteInt(nrunaway);  
	outputPacket.WriteInt(player->source);  
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->pid);  
	//TODO:
	outputPacket.WriteInt(0); 
	outputPacket.WriteInt(table->ante);  
	outputPacket.WriteInt(Configure::getInstance()->server_id);  
	outputPacket.WriteInt(table->id);  
	short clevel = table->clevel;
	//if(table->clevel == 6 || table->clevel == 7)
	//	clevel = 3;
	outputPacket.WriteInt(clevel);  
	outputPacket.WriteString(table->getGameID());  
	outputPacket.WriteInt(table->getEndType());  
	outputPacket.WriteInt64(player->money); 
	outputPacket.WriteInt(table->getStartTime());  
	outputPacket.WriteInt(time(NULL));  
	outputPacket.WriteInt(-1); 
	outputPacket.WriteInt(0); 
	outputPacket.WriteInt(player->m_nMagicNum); 
	outputPacket.WriteInt(player->m_nMagicCoinCount); 
	outputPacket.End();

	_LOG_DEBUG_("==>[updateDB] [0x%04x] [updateDB_UserCoin]\n", sedcmd);

	MySqlConnect* connect = MySqlConnect::getInstance();
	if(connect->Send( &outputPacket )==0)
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
 
	RoundServerConnect* connect = RoundServerConnect::getInstance();
	if(connect->Send( &outputPacket )==0)
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

int IProcess::updateDB_UserVoucher(Player* player, Table* table, int nroll, bool isboardtask)
{

	MoneyNodes* pmconnect = MoneyConnectManager::getNodes(player->id % 10 + 1);
	OutputPacket respone;
	respone.Begin(UPDATE_ROLL);
	respone.WriteInt(player->id); 
	respone.WriteByte(1);   
	respone.WriteInt(nroll);
	respone.End();
	if(pmconnect->send(&respone, false) < 0)
		_LOG_ERROR_("Send request to MoneyServer Error\n" );

	int sedcmd = 0x0122;
	OutputPacket outputPacket;
	outputPacket.Begin(sedcmd);
	outputPacket.WriteInt(player->source);
	outputPacket.WriteInt(player->cid);  
	outputPacket.WriteInt(player->pid);  
	outputPacket.WriteInt(player->sid);  
	outputPacket.WriteInt(player->id);  
	outputPacket.WriteInt(nroll); 
	//获得乐券的类型
	outputPacket.WriteInt(isboardtask ? 3 : 4);
	outputPacket.WriteInt(player->voucher); 
	//是局数任务直接写64就可以，因为64这个数字牌型任务不可能用到
	outputPacket.WriteInt64(isboardtask ? 64 : player->task1->taskid);  
	outputPacket.WriteInt(Configure::getInstance()->level);
	outputPacket.WriteInt(time(NULL));
	outputPacket.End();

	_LOG_DEBUG_("==>[updateDB] [0x%04x] [updateDB_UserVoucher]\n", sedcmd);

	MySqlConnect* connect = MySqlConnect::getInstance();
	if(connect->Send( &outputPacket )==0)
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

