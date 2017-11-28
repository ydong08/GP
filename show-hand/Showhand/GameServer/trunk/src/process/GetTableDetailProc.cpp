#include <string>
#include "GetTableDetailProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "UdpManager.h"
#include "LogServerConnect.h"

using namespace std;


GetTableDetailProc::GetTableDetailProc()
{
	this->name = "GetTableDetailProc" ;
}

GetTableDetailProc::~GetTableDetailProc()
{
}

int GetTableDetailProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[GetTableDetailProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);

	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		//用户离开，告诉userserver
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2, ErrorMsg::getInstance()->getErrMsg(-2), seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		//用户离开，告诉userserver
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1, ErrorMsg::getInstance()->getErrMsg(-1), seq);
	}

	if(table->isActive())
	{
		Json::Value data;
		data["BID"] = string(table->getGameID());
		data["Time"]=(int)(time(NULL) - table->getStartTime());
		data["currd"] = table->currRound;
		data["getID"] = player->id;
		data["count"] = (double)player->betCoinList[0];
		if(!table->isAllRobot())
			_LOG_REPORT_(player->id, RECORD_GET_DETAIL, "%s", data.toStyledString().c_str());
		data["errcode"] = 0;
		_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());
	}

	player->hallid = hallhandler->hallid;
	player->isdropline = false;

	Player* betplayer = table->getPlayer(table->betUid);
	short remaintime = 0;
	//当前超时时间
	if(betplayer)
		remaintime = Configure::getInstance()->betcointime - (time(NULL)-betplayer->GetBetCoinTime());
	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteByte(player->hascard ? player->card_array[0] : 0);
	response.WriteShort(table->currRound);
	response.WriteInt64(table->ante);
	response.WriteInt64(table->thisGameLimit);
	short currRound = table->currRound;
	response.WriteInt(table->betUid);
	response.WriteShort(betplayer ? betplayer->optype : 0);	
	response.WriteShort(remaintime);
	response.WriteShort(Configure::getInstance()->betcointime - 3);
	response.WriteShort(table->leavercount);
	for(int j = 0; j < table->leavercount; ++j)
	{
		response.WriteInt(table->leaverarry[j].uid);
		response.WriteShort(table->leaverarry[j].tab_index);
		response.WriteString(table->leaverarry[j].name);
		response.WriteInt64(table->leaverarry[j].betcoin);
	}
	response.WriteShort(table->countPlayer);

	_LOG_DEBUG_("<==[GetTableDetailProc] Push [0x%04x]\n", cmd);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] push to status=[%d]\n", player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] currRound=[%d]\n", table->currRound);
	_LOG_DEBUG_("[Data Response] betUid=[%d]\n", table->betUid);
	_LOG_DEBUG_("[Data Response] betoptype=[%d]\n", betplayer ? betplayer->optype : 0);
	_LOG_DEBUG_("[Data Response] remaintime=[%d]\n", remaintime);
	_LOG_DEBUG_("[Data Response] betcointime=[%d]\n", Configure::getInstance()->betcointime - 3);
	_LOG_DEBUG_("[Data Response] leavercount=[%d]\n", table->leavercount);
	_LOG_DEBUG_("[Data Response] countPlayer=[%d]\n", table->countPlayer);
	int i= 0;
	int sendnum = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->countPlayer)
			break;
		Player* getPlayer = table->player_array[i];
		if(getPlayer)
		{
			response.WriteInt(getPlayer->id);
			response.WriteString(getPlayer->name);
			response.WriteShort(getPlayer->status);
			response.WriteShort(getPlayer->tab_index);
			response.WriteInt64(getPlayer->money);
			response.WriteInt64(getPlayer->carrycoin);
			response.WriteInt64(getPlayer->betCoinList[0]);
			response.WriteInt64(getPlayer->betCoinList[table->currRound]);
			response.WriteInt64(table->currMaxCoin - getPlayer->betCoinList[table->currRound]);
			response.WriteInt64(getPlayer->nextlimitcoin);
			response.WriteInt(getPlayer->nwin);
			response.WriteInt(getPlayer->nlose);
			response.WriteString(getPlayer->json);
			_LOG_DEBUG_("[Data Response] getPlayerid=[%d]\n", getPlayer->id);
			_LOG_DEBUG_("[Data Response] getPlayer->status=[%d]\n", getPlayer->status);
			_LOG_DEBUG_("[Data Response] getPlayer->tab_index=[%d]\n", getPlayer->tab_index);
			_LOG_DEBUG_("[Data Response] getPlayer->money=[%ld]\n", getPlayer->money);
			_LOG_DEBUG_("[Data Response] getPlayer->carrycoin=[%ld]\n", getPlayer->carrycoin);
			_LOG_DEBUG_("[Data Response] getPlayer->betCoinList[0]=[%ld]\n", getPlayer->betCoinList[0]);
			_LOG_DEBUG_("[Data Response] getPlayer->betCoinList[table->currRound]=[%ld]\n", getPlayer->betCoinList[table->currRound]);
			_LOG_DEBUG_("[Data Response] callmoney=[%ld]\n", table->currMaxCoin - getPlayer->betCoinList[table->currRound]);
			_LOG_DEBUG_("[Data Response] nextlimitcoin=[%ld]\n",getPlayer->nextlimitcoin);
			_LOG_DEBUG_("[Data Response] nwin=[%d]\n",getPlayer->nwin);
			_LOG_DEBUG_("[Data Response] nlose=[%d]\n",getPlayer->nlose);
			if(getPlayer->hascard)
			{
				response.WriteShort(1);
				_LOG_DEBUG_("[Data Response] hascard=[%d]\n", 1);
				//当前还没有到所有要亮底牌的时候
				if(currRound != 6)
				{
					//如果第五轮则发牌也只是发明牌的
					if(currRound == 5)
					{
						for(int j = 1; j < 5; j++)
							response.WriteByte(getPlayer->card_array[j]);
					}
					else
					{
						for(int j = 1; j < currRound + 1; j++)
							response.WriteByte(getPlayer->card_array[j]);
					}
					//返回当前用户手牌类型
					short cardType = table->GetPlayerCardKind(getPlayer->card_array, 1, currRound == 5 ? 4 : currRound);
					response.WriteShort(cardType); 
				}
				else
				{
					//如果是第六轮直接把所有玩家的底牌都发过去
					for(int j = 0; j < 5; j++)
						response.WriteByte(getPlayer->card_array[j]);
					//返回当前用户手牌类型
					short cardType = table->GetPlayerCardKind(getPlayer->card_array, 0, 4);
					response.WriteShort(cardType); 
				}
			}
			else
				response.WriteShort(0);

			sendnum++;
		}
	}
	if(player->task1)
	{
		response.WriteInt64(player->task1->taskid);
		response.WriteString(player->source == 1 ? player->task1->taskANDname : player->task1->taskIOSname);
		response.WriteShort(player->ningot);
		response.WriteInt(player->voucher);
	}
	else
	{
		response.WriteInt64(0);
		response.WriteString("");
		response.WriteShort(0);
		response.WriteInt(0);
	}

	if(player->istask && !player->isroundtask)
	{
		int ntask = player->boardTask;
		int nlow = ntask & 0x00FF;
		int nmid = ntask>>8 & 0x00FF;
		int nhigh = ntask>>16 & 0x00FF;
		int ncomplete = ntask >> 24;
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
	}

	if(player->isroundtask)
	{
		response.WriteShort(0);
		response.WriteString("");
		response.WriteShort(0);
		response.WriteShort(0);
		response.WriteShort(0);
		int comflag = 0;
		int roundnum = 0;
		if(Configure::getInstance()->level == 1)
		{
			comflag = player->m_nRoundComFlag & 0x0F;
			roundnum = player->m_nRoundNum & 0x00FF;
		}
		if(Configure::getInstance()->level == 2)
		{
			comflag = (player->m_nRoundComFlag >> 4) & 0x0F;
			roundnum = (player->m_nRoundNum >> 8) & 0x00FF;
		}
		if(Configure::getInstance()->level == 3)
		{
			comflag = (player->m_nRoundComFlag >> 8) & 0x0F;
			roundnum = (player->m_nRoundNum >> 16) & 0x00FF;
		}

		_LOG_DEBUG_("comflag:%d roundnum:%d\n", comflag, roundnum);

		if(table->m_nRoundNum3 != 0)
		{
			_LOG_DEBUG_("m_nRoundNum3:%d m_nRoundNum2:%d m_nRoundNum1:%d\n", table->m_nRoundNum3, table->m_nRoundNum2, table->m_nRoundNum1);
			//都完成了
			if(comflag & 4)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//完成中间的
			else if (comflag & 2)
			{
				response.WriteByte(roundnum - (table->m_nRoundNum1 + table->m_nRoundNum2));
				response.WriteByte(table->m_nRoundNum3);
			}
			//完成第一个局数
			else if (comflag & 1)
			{
				response.WriteByte(roundnum - table->m_nRoundNum1);
				response.WriteByte(table->m_nRoundNum2);
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
			}
		}
		else if(table->m_nRoundNum2 != 0)
		{
			//完成中间的
			if (comflag & 2)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//完成第一个局数
			else if (comflag & 1)
			{
				response.WriteByte(roundnum - table->m_nRoundNum1);
				response.WriteByte(table->m_nRoundNum2);
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
			}
		}
		else if (table->m_nRoundNum1 != 0)
		{
			//完成第一个局数
			if (comflag & 1)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
			}
		}
		else
		{
			response.WriteByte(0);
			response.WriteByte(0);
		}
		response.WriteByte(table->m_bRewardType);
		if(table->m_bRewardType == 2)
		{
			response.WriteInt(player->m_nComGetRoll);
			response.WriteInt(Configure::getInstance()->rewardroll);
		}
		else
		{
			response.WriteInt(player->m_nComGetCoin);
			response.WriteInt(Configure::getInstance()->rewardcoin);
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[GetTableDetailProc] Send To Uid[%d] Error!\n", player->id);

	return 0;
}

int GetTableDetailProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}



