#include "GetTableDetailProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "IProcess.h"

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetTableDetailProc)


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
		limittime = Configure::getInstance().idletime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isBet())
	{
		limittime = Configure::getInstance().bettime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isOpen())
	{
		limittime = Configure::getInstance().opentime - (time(NULL) - table->getStatusTime());
	}

	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(Configure::getInstance().m_nLevel);
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
	for(i = 1; i < 5; ++i)
		response.WriteInt64(table->m_lTabBetArray[i]);
	_LOG_DEBUG_("+++++++++++++m_lTabBetArray:%d m_nSeatID:%d \n", table->m_lTabBetArray[0], player->m_nSeatID);
	Player* banker = NULL;
	if(table->bankersid >=0)
		banker = table->player_array[table->bankersid];
	response.WriteInt(banker ? banker->id : 0);
	response.WriteShort(banker ? banker->m_nSeatID : -1);
	response.WriteString(banker ? banker->name : "");
	response.WriteInt64(banker ? banker->m_lMoney : 0);
	response.WriteString(banker ? banker->json : "");
	response.WriteByte(Configure::getInstance().bankernum - table->bankernum);
	response.WriteByte(player->isbankerlist ? 1: 0);
	response.WriteInt64(Configure::getInstance().bankerlimit);
	if(table->isOpen())
	{
		for(int m=0; m < 5; ++m)
		{
			for(int n = 0; n < MAX_COUNT; ++n)
			{
				response.WriteByte(table->m_bTableCardArray[m][n]);
				_LOG_DEBUG_("m:%d n:%d cardvalue:0x%02x\n", m, n, table->m_bTableCardArray[m][n]);
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
					_LOG_DEBUG_("tian:%d\n",table->m_GameRecordArrary[index].cbTian);
				}
				if(m == 2)
				{
					response.WriteByte(table->m_GameRecordArrary[index].cbDi);
					_LOG_DEBUG_("di:%d\n",table->m_GameRecordArrary[index].cbDi);
				}
				if(m == 3)
				{
					response.WriteByte(table->m_GameRecordArrary[index].cbXuan);
					_LOG_DEBUG_("xuan:%d\n",table->m_GameRecordArrary[index].cbXuan);
				}
				if(m == 4)
				{
					response.WriteByte(table->m_GameRecordArrary[index].cbHuang);
					_LOG_DEBUG_("huang:%d\n",table->m_GameRecordArrary[index].cbHuang);
				}
			}
		}
	}
	response.WriteInt64(player->m_lWinScore);
	response.WriteInt64(table->m_lBankerWinScoreCount);
	response.WriteByte(MAX_MUL);
	response.WriteInt(table->m_nLimitCoin);
	response.WriteInt64(player->m_lReturnScore);
	response.WriteInt64(table->m_lJackPot);
	response.WriteByte(MAX_MUL);
	response.WriteByte(MAX_MUL);
	response.WriteByte(8);
	response.WriteByte(6);
	response.WriteByte(4);
	response.WriteByte(2);
	response.WriteByte(1);
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[LoginComingProc] Send To Uid[%d] Error!\n", player->id);

	table->SendSeatInfo(player);

	return 0;
}

int GetTableDetailProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
