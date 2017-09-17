#include "StandupProc.h"
#include "Logger.h"
#include "Room.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "ErrorMsg.h"
#include "IProcess.h"

StandupProc::StandupProc()
{
	this->name = "StandupProc";
}

StandupProc::~StandupProc()
{
}

int StandupProc::doRequest(CDLSocketHandler * clientHandler, InputPacket * pPacket, Context * pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	//0表示离座，1表示就座，2表示换座，3庄家申请离庄
	int action = pPacket->ReadByte();
	int seatid = pPacket->ReadByte();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;

	LOGGER(E_LOG_INFO) << "standup_request_params ="
		<< " cmd = " << TOHEX(cmd)
		<< " uid = " << uid
		<< " tid = " << tid
		<< " action = " << action
		<< " seatid = " << seatid
		<< " svid = " << svid
		<< " realTid = " << realTid;

	// 桌子不存在
	Table * pTable = Room::getInstance()->getTable(realTid);
	if (pTable == NULL)
	{
		LOGGER(E_LOG_ERROR) << "Table isn't exist!";
		return sendErrorMsg(clientHandler, cmd, uid, -2, ERRMSG(-2), seq);
	}

	// 玩家不存在
	Player *pPlayer = pTable->getPlayer(uid);
	if (pPlayer == NULL)
	{
		ULOGGER(E_LOG_ERROR, uid) << "Player isn't exist!";
		return sendErrorMsg(clientHandler, cmd, uid, -1, ERRMSG(-1), seq);
	}

	// 玩家正在打牌
	if (pPlayer->isActive())
	{
		ULOGGER(E_LOG_ERROR, uid) << "Player status not allowd, u_status = " << pPlayer->m_nStatus << ", t_status = " << pTable->m_nStatus;
		return sendErrorMsg(clientHandler, cmd, uid, -41, ERRMSG(-41), seq);
	}

	// 庄家不能站起
	if (pTable->bankeruid == pPlayer->id)
	{
		ULOGGER(E_LOG_ERROR, uid) << "Banker is not allowd";
		return sendErrorMsg(clientHandler, cmd, uid, -45, ERRMSG(-45), seq);
	}

	int ret = 0;
	if (action == 0)
		ret = pTable->playerStandUp(pPlayer);
	if (action == 1)
		ret = pTable->playerSeatDown(pPlayer, seatid);
	//if (action == 2)
	//	ret = pTable->playerChangeSeat(pPlayer, seatid);
	if (ret)
		return sendErrorMsg(clientHandler, cmd, uid, ret, ERRMSG(ret), seq);

	IProcess::sendTakeSeatInfo(pTable, pPlayer, action, seatid, seq);

	pPlayer->setActiveTime(time(NULL));

	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_STANDUP, StandupProc);