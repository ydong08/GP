#include "ApplySeatProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "BaseClientHandler.h"
#include "Configure.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "CoinConf.h"

/*
	int uid;
	int tid;
	int seatid;
 */

/*
    short ret;
	std::string retmsg;
	int   uid;
	int64 limit;
	int64 money;
------
	
 */

ApplySeatProc::ApplySeatProc()
{
	this->name = "ApplySeatProc";
}

ApplySeatProc::~ApplySeatProc()
{

} 

int ApplySeatProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	int seatid = pPacket->ReadInt();
	_LOG_INFO_("==>[ApplySeatProc]  cmd[0x%04x] uid[%d] tid[%d] seatid[%d]\n", cmd, uid, tid, seatid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	Room* room = Room::getInstance();
	Table *table = room->getTable();
	if(table==NULL)
	{
		_LOG_ERROR_("ApplySeatPorc: table is NULL\n");
		OutputPacket response;
		response.Begin(cmd, uid);
		response.SetSeqNum(seq);
		response.WriteShort(-2);
		response.WriteString(ErrorMsg::getInstance()->getErrMsg(-2));
		response.WriteInt(uid);
		response.WriteInt64(0);
		response.WriteInt64(0);
		response.End();
		if(hallhandler)
			hallhandler->Send(&response,false);
		return 0;
	}

	Player* player = table->getPlayer(uid);
	if (player == NULL)
	{
		_LOG_ERROR_("ApplySeatPorc: uid[%d] Player is NULL\n", uid);
		//sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(-1);
		response.WriteString(ErrorMsg::getInstance()->getErrMsg(-1));
		response.WriteInt(uid);
		response.WriteInt64(0);
		response.WriteInt64(0);
		response.End();
		if(hallhandler)
			hallhandler->Send(&response,false);
		return 0;
	}

	if(player->m_lMoney < table->m_nLimitCoin * MAX_SEAT_MUL)
	{
		char		acErrorMessage[256];

		_LOG_ERROR_("Your uid[%d] m_lMoney[%ld] is not enough high[%d]\n", uid, player->m_lMoney, CoinConf::getInstance()->getCoinCfg()->bankerlimit);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(-25);
		sprintf(acErrorMessage, ErrorMsg::getInstance()->getErrMsg(-25), table->m_nLimitCoin * MAX_SEAT_MUL / 100.0);
		response.WriteString(std::string(acErrorMessage));
		response.WriteInt(uid);
		response.WriteInt64(table->m_nLimitCoin * MAX_SEAT_MUL);
		response.WriteInt64(player->m_lMoney);
		response.End();
		if(hallhandler)
			hallhandler->Send(&response,false);
		return 0;
	}
		
	player->setActiveTime(time(NULL));
	if (table->EnterSeat(seatid, player))
	{
		_LOG_INFO_("ApplySeatProc::doRequest : Enter Success uid=[%d] seat_id=[%d]\n", uid, seatid);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(0);
		response.WriteString("");
		response.WriteInt(uid);
		response.WriteInt64(table->m_nLimitCoin * MAX_SEAT_MUL);
		response.WriteInt64(player->m_lMoney);
		response.End();
		if(hallhandler)
			hallhandler->Send(&response,false);

		table->NotifyPlayerSeatInfo();
	}
	else
	{	
		_LOG_ERROR_("ApplySeatProc::doRequest : Error uid[%d] enter seatid[%d] \n", uid, seatid);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(-26);
		response.WriteString(ErrorMsg::getInstance()->getErrMsg(-26));
		response.WriteInt(uid);
		response.WriteInt64(table->m_nLimitCoin * MAX_SEAT_MUL);
		response.WriteInt64(player->m_lMoney);
		response.End();
		if(hallhandler)
			hallhandler->Send(&response,false);
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_APPLY_SEAT, ApplySeatProc)