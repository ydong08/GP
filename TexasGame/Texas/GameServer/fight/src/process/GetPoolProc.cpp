#include <string>
#include "GetPoolProc.h"
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


GetPoolProc::GetPoolProc()
{
	this->name = "GetPoolProc" ;
}

GetPoolProc::~GetPoolProc()
{
}

int GetPoolProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[GetPoolProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[GetPoolProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[GetPoolProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	/*if(!table->isActive())
	{
		_LOG_ERROR_("[GetPoolProc] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(hallhandler, cmd, uid, -11,ErrorMsg::getInstance()->getErrMsg(-11),seq);
	}*/

	int i = 0;
	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(table->m_bCurrRound);
	if(table->m_PoolArray[0].betCoinCount > 0)
	{
		short poolnum = table->m_nPoolNum;
		if(table->m_PoolArray[poolnum].betCoinCount > 0)
			poolnum = table->m_nPoolNum + 1;
		response.WriteByte(poolnum);
		for(i = 0; i < poolnum; ++i)
		{
			response.WriteInt64(table->m_PoolArray[i].betCoinCount);
			printf("i:%d betcoincount:%ld\n", i, table->m_PoolArray[i].betCoinCount);
		}
	}
	else
		response.WriteByte(0);
	response.WriteInt64(table->getSumPool());
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[sendPoolinfo] Send To Uid[%d] Error!\n", player->id);

	_LOG_DEBUG_("<==[sendPoolinfo] Push [0x%04x] \n", CLIENT_GET_POOL);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_nPoolNum=[%d]\n", table->m_nPoolNum+1);
	_LOG_DEBUG_("[Data Response] SumPoolNum=[%ld]\n", table->getSumPool());

	return 0;
}

int GetPoolProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

REGISTER_PROCESS(CLIENT_GET_POOL, GetPoolProc)

