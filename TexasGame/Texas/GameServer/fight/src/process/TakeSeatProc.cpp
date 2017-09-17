#include "TakeSeatProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "IProcess.h"
#include "ProcessManager.h"
#include "GameCmd.h"


REGISTER_PROCESS(CLIENT_MSG_TAKESEAT, TakeSeatProc)

TakeSeatProc::TakeSeatProc()
{
	this->name = "TakeSeatProc";
}

TakeSeatProc::~TakeSeatProc()
{

}

int TakeSeatProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	//short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	//0表示离座，1表示就座，2表示换座，3庄家申请离庄
	int action = pPacket->ReadByte();
	int seatid = pPacket->ReadByte();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_DEBUG_("==>[TakeSeatProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d] action[%d] seatid[%d]\n", tid, svid, realTid, action, seatid);
	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[TakeSeatProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[TakeSeatProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	if(player->isActive())
	{
		_LOG_ERROR_("[TakeSeatProc] uid=[%d] ustatus=[%d] tid=[%d] realTid=[%d] Table status[%d] in active\n",uid, player->m_nStatus, tid, realTid, table->m_nStatus);
		return sendErrorMsg(clientHandler, cmd, uid, -41,ErrorMsg::getInstance()->getErrMsg(-41),seq);
	}

	_LOG_INFO_("[TakeSeatProc] UID=[%d] GameID=[%s] tid=[%d] realTid=[%d] TakeSeatProc OK\n", uid, table->getGameID(), tid, realTid);

	int ret = 0;
	if (action == 0)
		ret = table->playerStandUp(player);
	if (action == 1)
		ret = table->playerSeatDown(player, seatid);
	if (action == 2)
		ret = table->playerChangeSeat(player, seatid);
	if (ret)
		return sendErrorMsg(clientHandler, cmd, uid, ret,ErrorMsg::getInstance()->getErrMsg(ret), seq);

	IProcess::sendTakeSeatInfo(table, player, action, seatid, seq);

	player->setActiveTime(time(NULL));

	return 0;
}

int TakeSeatProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
