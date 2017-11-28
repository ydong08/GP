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

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] Table is NULL\n",uid, tid);
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		return sendErrorMsg(clientHandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[GetTableDetailProc] uid=[%d] tid=[%d] Your not in This Table\n",uid, tid);
		AllocSvrConnect::getInstance()->userLeaveGame(NULL, uid);
		return sendErrorMsg(clientHandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	player->m_nHallid = hallhandler->hallid;
	player->isonline = true;

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
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(Configure::getInstance()->m_nServerId << 16 | table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(limittime);
	response.WriteShort(table->getPlayerCount(false));
	response.WriteShort(player->m_nSeatID);
	response.WriteInt64(player->m_lMoney);
	int i = 0;
	for(i = 1; i < BETNUM; ++i)
		response.WriteInt64(player->m_lBetArray[i]);
	for(i = 1; i < BETNUM; ++i)
		response.WriteInt64(table->m_lTabBetArray[i]);
	Player* banker = NULL;
	banker = table->getBanker();
	response.WriteInt(banker ? banker->id : 0);
	response.WriteShort(banker ? banker->m_nSeatID : -1);
	response.WriteString(banker ? banker->name : "");
	response.WriteInt64(banker ? banker->m_lMoney : 0);
	response.WriteString(banker ? banker->json : "");
	response.WriteByte((BYTE)(table->m_nMaxBankerNum - table->bankernum));
	response.WriteByte(table->isWaitForBanker(uid) ? 1: 0);
	response.WriteInt64(table->m_nBankerlimit);
	response.WriteInt(table->m_nLimitCoin);
	response.WriteInt64(player->m_lReturnScore);
    for(i = 1; i < BETNUM; ++i)
        response.WriteInt64(table->betAreaLimit[i]);
    response.WriteInt(table->m_nMinBetNum);
    response.WriteInt(table->m_nMaxBetNum);
    for (int i = 0; i < CHIP_COUNT; i++)
    {
        response.WriteInt(table->m_nChipArray[i]);
    }
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[LoginComingProc] Send To Uid[%d] Error!\n", player->id);
	
	table->SendSeatInfo(player);

	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetTableDetailProc);
