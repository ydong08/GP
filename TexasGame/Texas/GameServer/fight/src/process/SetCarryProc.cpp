#include <string>
#include "SetCarryProc.h"
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


SetCarryProc::SetCarryProc()
{
	this->name = "SetCarryProc" ;
}

SetCarryProc::~SetCarryProc()
{
}

int SetCarryProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	int64_t addmoney = pPacket->ReadInt64();
	short buyinflag = pPacket->ReadByte();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[SetCarryProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d] addmoney[%ld]\n", tid, svid, realTid, addmoney);
	_LOG_DEBUG_("[DATA Parse] buyinflag=[%d] \n", buyinflag);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();
	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[SetCarryProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[SetCarryProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	/*if(!table->isActive())
	{
		_LOG_ERROR_("[SetCarryProc] uid=[%d] tid=[%d] realTid=[%d] tm_nStatus=[%d] is not active\n",uid, tid, realTid, table->m_nStatus);
		return sendErrorMsg(hallhandler, cmd, uid, -11,ErrorMsg::getInstance()->getErrMsg(-11),seq);
	}*/
	if(buyinflag != 1)
	{
		int ret = player->setCarryInNextRound(addmoney);
		
		if(ret == -1)
		{
			_LOG_ERROR_("[SetCarryProc] uid=[%d] tid=[%d] realTid=[%d] ret=[%d]\n",uid, tid, realTid, ret);
			return sendErrorMsg(hallhandler, cmd, uid, -15,ErrorMsg::getInstance()->getErrMsg(-15),seq);
		}
		if(ret == -2)
		{
			_LOG_ERROR_("[SetCarryProc] uid=[%d] tid=[%d] realTid=[%d] ret=[%d]\n",uid, tid, realTid, ret);
			return sendErrorMsg(hallhandler, cmd, uid, -16,ErrorMsg::getInstance()->getErrMsg(-16),seq);
		}
		if(ret == -3)
		{
			_LOG_ERROR_("[SetCarryProc] uid=[%d] tid=[%d] realTid=[%d] ret=[%d]\n",uid, tid, realTid, ret);
			return sendErrorMsg(hallhandler, cmd, uid, -17,ErrorMsg::getInstance()->getErrMsg(-17),seq);
		}
	}
	Json::Value data;
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());
	player->m_nBuyInFlag = buyinflag;

	OutputPacket response;
	response.Begin(cmd, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteInt64(player->m_lMoney);
	response.WriteInt64(player->m_lCarryMoney);
	response.WriteInt64(addmoney);
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[SetCarryProc] Send To Uid[%d] Error!\n", player->id);

	_LOG_DEBUG_("<==[SetCarryProc] Push [0x%04x] \n", CLIENT_SET_CARRY);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[ok]\n");
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);

	return 0;
}

int SetCarryProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

REGISTER_PROCESS(CLIENT_SET_CARRY, SetCarryProc)

