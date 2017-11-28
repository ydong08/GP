#include "GetTableDetailProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "BaseClientHandler.h"
#include "AllocSvrConnect.h"
#include "Protocol.h"
#include "ProcessManager.h"
#include "CoinConf.h"

GetTableDetailProc::GetTableDetailProc()
{
	this->name = "GetTableDetailProc";
}

GetTableDetailProc::~GetTableDetailProc()
{

} 

int GetTableDetailProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[GetTableDetailProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable();

	if(table == NULL)
	{
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] Table is NULL\n",uid, tid);
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] Your not in This Table\n",uid, tid);
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	player->m_nHallid = hallhandler->hallid;
	player->isonline = true;

	_LOG_INFO_("[GetTableDetailProc] UID=[%d] tid=[%d] realTid=[%d] GetTableDetailProc OK\n", uid, tid, realTid);

	short limittime = 0;
	if (table->isIdle())
	{
		limittime = Configure::getInstance()->idletime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isBet())
	{
		limittime = Configure::getInstance()->bettime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isOpen())
	{
		limittime = Configure::getInstance()->opentime - (time(NULL) - table->getStatusTime());
	}

	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(Configure::getInstance()->m_nLevel);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(limittime);
	response.WriteShort(table->m_nCountPlayer);
	response.WriteShort(player->m_nSeatID);
	response.WriteInt64(player->m_lMoney);
	int i = 0;
	for(i = 1; i < 5; ++i)
		response.WriteInt64(player->m_lBetArray[i]);
	for (i = 1; i < 5; ++i)
	{
		response.WriteInt64(table->m_lTabBetArray[i]);
		LOGGER(E_LOG_INFO) << "notify_client, bettype = " << i << " table_bet_num = " << table->m_lTabBetArray[i];
	}
	Player* banker = NULL;
	if(table->bankersid >=0)
		banker = table->player_array[table->bankersid];
	response.WriteInt(banker ? banker->id : 0);
	response.WriteShort(banker ? banker->m_nSeatID : -1);
	response.WriteString(banker ? banker->name : "");
	response.WriteInt64(banker ? banker->m_lMoney : 0);
	if (source == E_MSG_SOURCE_ROBOT)
	{
		for (int i = 1; i < BETNUM; i++)
		{
			response.WriteInt64(table->m_lRealBetArray[i]);
			LOGGER(E_LOG_INFO) << "notify_client, bettype = " << i << " real bet num = " << table->m_lRealBetArray[i];
		}
	}
	response.WriteString(banker ? banker->json : "");
	response.WriteByte(CoinConf::getInstance()->getCoinCfg()->bankernumplayer - table->bankernum);
	response.WriteByte(player->isbankerlist ? 1: 0);
	response.WriteInt64(CoinConf::getInstance()->getCoinCfg()->bankerlimit);
	if(table->isOpen())
	{
		for(int m=0; m < 5; ++m)
		{
			for(int n = 0; n < 5; ++n)
			{
				response.WriteByte(table->m_bTableCardArray[m][n]);
				//_LOG_DEBUG_("m:%d n:%d cardvalue:0x%02x\n", m, n, table->m_bTableCardArray[m][n]);
			}
			response.WriteByte(table->m_CardResult[m].type);
			response.WriteByte(table->m_CardResult[m].mul);
			if(m > 0)
			{
				response.WriteInt64(player->m_lResultArray[m]);
				int index = table->m_nRecordLast -1;
				index = index < 0 ? MAX_SCORE_HISTORY -1 : index;
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
	}
	response.WriteInt64(player->m_lWinScore);
	response.WriteInt64(table->m_lBankerWinScoreCount);
	response.WriteByte(10);
	response.WriteInt(table->m_nLimitCoin);
	response.WriteInt64(player->m_lReturnScore);

	Cfg *pCoinCfg = CoinConf::getInstance()->getCoinCfg();
	if (pCoinCfg != NULL)
	{
		response.WriteInt(4);	// count
		response.WriteInt(pCoinCfg->raise1);
		response.WriteInt(pCoinCfg->raise2);
		response.WriteInt(pCoinCfg->raise3);
		response.WriteInt(pCoinCfg->raise4);
		LOGGER(E_LOG_INFO) << "Restore raise info success!";
	}
	else
	{
		response.WriteInt(4);	// count
		response.WriteInt(0);
		response.WriteInt(0);
		response.WriteInt(0);
		response.WriteInt(0);
		LOGGER(E_LOG_INFO) << "Restore raise info failed!";
	}

	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetTableDetailProc] Send To Uid[%d] Error!\n", player->id);
	
	table->SendSeatInfo(player);
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetTableDetailProc)