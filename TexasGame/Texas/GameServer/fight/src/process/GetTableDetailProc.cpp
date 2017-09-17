#include <string>
#include "GetTableDetailProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "AllocSvrConnect.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "BaseClientHandler.h"
#include "json/json.h"

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

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		//用户离开，告诉userserver
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		//用户离开，告诉userserver
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	if(player->isActive())
	{
		Json::Value data;
		data["BID"] = string(table->getGameID());
		data["Time"]=(int)(time(NULL) - table->getStartTime());
		data["currd"] = table->m_bCurrRound;
		data["getID"] = player->id;
		data["count"] = (double)player->betCoinList[0];
		if(!table->isAllRobot())
			_LOG_REPORT_(player->id, RECORD_GET_DETAIL, "%s", data.toStyledString().c_str());
		data["errcode"] = 0;
		_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());
	}

	player->m_nHallid = hallhandler->hallid;
	player->isdropline = false;
	int i= 0;
	Player* betplayer = table->getPlayer(table->m_nCurrBetUid);
	short remaintime = 0;
	//当前超时时间
	if(betplayer)
		remaintime = Configure::getInstance()->betcointime - (time(NULL) - betplayer->GetBetCoinTime());
	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(player->card_array[0]);
	response.WriteByte(player->card_array[1]);
	response.WriteByte(table->m_bCurrRound);
	if(table->m_bCurrRound < 2)
	{
		response.WriteByte(0);
		response.WriteByte(0);
		response.WriteByte(0);
		response.WriteByte(0);
		response.WriteByte(0);
	}
	if(table->m_bCurrRound == 2)
	{
		response.WriteByte(table->m_cbCenterCardData[0]);
		response.WriteByte(table->m_cbCenterCardData[1]);
		response.WriteByte(table->m_cbCenterCardData[2]);
		response.WriteByte(0);
		response.WriteByte(0);
	}
	if(table->m_bCurrRound == 3)
	{
		response.WriteByte(table->m_cbCenterCardData[0]);
		response.WriteByte(table->m_cbCenterCardData[1]);
		response.WriteByte(table->m_cbCenterCardData[2]);
		response.WriteByte(table->m_cbCenterCardData[3]);
		response.WriteByte(0);
	}	
	if(table->m_bCurrRound >= 4)
	{
		response.WriteByte(table->m_cbCenterCardData[0]);
		response.WriteByte(table->m_cbCenterCardData[1]);
		response.WriteByte(table->m_cbCenterCardData[2]);
		response.WriteByte(table->m_cbCenterCardData[3]);
		response.WriteByte(table->m_cbCenterCardData[4]);
	}
	response.WriteInt64(table->m_lBigBlind);
	response.WriteInt64(table->m_lGameLimt);
	response.WriteInt64(table->m_lCurrMax);
	response.WriteInt64(table->getSumPool());
	response.WriteInt64(player->betCoinList[table->m_bCurrRound]);
	response.WriteInt64(player->betCoinList[0]);
	response.WriteShort(player->optype);
	response.WriteInt64(player->m_lBetLimit);

	if(table->m_PoolArray[0].betCoinCount > 0)
	{
		short poolnum = table->m_nPoolNum;
		if(table->m_PoolArray[poolnum].betCoinCount > 0)
			poolnum = table->m_nPoolNum + 1;
		response.WriteByte(poolnum);
		for(i = 0; i < poolnum; ++i)
			response.WriteInt64(table->m_PoolArray[i].betCoinCount);
	}
	else
		response.WriteByte(0);
	response.WriteInt(table->m_nCurrBetUid);
	response.WriteByte(table->m_nDealerIndex);
	response.WriteByte(remaintime>16 ? remaintime-3 : remaintime);
	response.WriteByte(Configure::getInstance()->betcointime - 3);
	response.WriteByte(table->m_nCountPlayer);

	_LOG_DEBUG_("<==[GetTableDetailProc] Push [0x%04x]\n", cmd);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] push to status=[%d]\n", player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] card1=[0x%02x] card2=[0x%02x]\n", player->card_array[0], player->card_array[1]);
	_LOG_DEBUG_("[Data Response] currRound=[%d]\n", table->m_bCurrRound);
	_LOG_DEBUG_("[Data Response] betUid=[%d]\n", table->m_nCurrBetUid);
	_LOG_DEBUG_("[Data Response] remaintime=[%d]\n", remaintime);
	_LOG_DEBUG_("[Data Response] betcointime=[%d]\n", Configure::getInstance()->betcointime - 3);
	_LOG_DEBUG_("[Data Response] countPlayer=[%d]\n", table->m_nCountPlayer);
	_LOG_DEBUG_("[Data Response] m_nDealerIndex=[%d]\n", table->m_nDealerIndex);
	_LOG_DEBUG_("[Data Response] getSumPool=[%d]\n", table->getSumPool());

	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* getPlayer = table->player_array[i];
		if(getPlayer)
		{
			response.WriteInt(getPlayer->id);
			response.WriteString(getPlayer->name);
			response.WriteShort(getPlayer->m_nStatus);
			response.WriteByte(getPlayer->m_bHasCard);
			response.WriteByte(getPlayer->m_nTabIndex);
			response.WriteInt64(getPlayer->m_lMoney);
			response.WriteInt64(getPlayer->m_lCarryMoney);
			//说明当前正在发牌倒计时中
			response.WriteInt64(table->hassendcard ? 0 : getPlayer->betCoinList[table->m_bCurrRound]);
			response.WriteInt(getPlayer->m_nWin);
			response.WriteInt(getPlayer->m_nLose);
			response.WriteString(getPlayer->json);
			/*_LOG_DEBUG_("[Data Response] getPlayerid=[%d]\n", getPlayer->id);
			_LOG_DEBUG_("[Data Response] getPlayer->m_nStatus=[%d]\n", getPlayer->m_nStatus);
			_LOG_DEBUG_("[Data Response] getPlayer->m_nTabIndex=[%d]\n", getPlayer->m_nTabIndex);
			_LOG_DEBUG_("[Data Response] getPlayer->money=[%ld]\n", getPlayer->m_lMoney);
			_LOG_DEBUG_("[Data Response] getPlayer->carrycoin=[%ld]\n", getPlayer->m_lCarryMoney);
			_LOG_DEBUG_("[Data Response] nwin=[%d]\n",getPlayer->m_nWin);
			_LOG_DEBUG_("[Data Response] nlose=[%d]\n",getPlayer->m_nLose);*/

		}
	}
	response.WriteShort(player->m_pTask ? player->m_nGetRoll : 0);
    response.WriteString(player->m_pTask ? player->m_pTask->taskname : "");
	response.WriteInt(player->m_nRoll);
	if(table->m_bIsOverTimer && table->m_bCurrRound == 5)
	{
		response.WriteByte(1);
		for(i = 0; i < table->m_bMaxNum; ++i)
		{
			Player* getPlayer = table->player_array[i];
			if(getPlayer)
			{
				response.WriteInt(getPlayer->id);
				response.WriteByte(getPlayer->card_array[0]);
				response.WriteByte(getPlayer->card_array[1]);
			}
		}
	}
	else
		response.WriteByte(0);
	response.WriteInt(table->m_nMustBetCoin);
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
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

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetTableDetailProc)