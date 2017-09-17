

#include "GetMoneyProc.h"
#include "GameCmd.h"
//#include "ProcessManager.h"
#include "Logger.h"
#include "ErrorMsg.h"
#include "Room.h"
#include "BaseClientHandler.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "IProcess.h"
#include "ProtocolClientId.h"

REGISTER_PROCESS(CLIENT_MSG_GET_MONEY, GetMoneyProc)


GetMoneyProc::GetMoneyProc()
{
	this->name = "GetMoneyProc";
}

GetMoneyProc::~GetMoneyProc()
{

}

int GetMoneyProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	int cmd = inputPacket->GetCmdType();
	short seq = inputPacket->GetSeqNum();
	int uid = inputPacket->ReadInt();
	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);
	ULOGGER(E_LOG_INFO, uid) << "cmd=" << cmd;
	Player* player = Room::getInstance()->getPlayerUID(uid);
	if (player == NULL)
	{
		ULOGGER(E_LOG_ERROR, uid) << "player == NULL";
		sendErrorMsg(hallhandler, cmd, uid, -1, ErrorMsg::getInstance()->getErrMsg(-1), seq);
		return 0;
	}

	OutputPacket response;
	response.Begin(cmd, uid);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(uid);
	response.WriteInt64(player->m_lMoney);
	response.WriteShort(1);	//获取身上的金币标识
	response.End();
	if (hallhandler->Send(&response, false) < 0)
	{
		ULOGGER(E_LOG_ERROR, uid) << "send to hall failed!";
		sendErrorMsg(hallhandler, cmd, uid, -2, ErrorMsg::getInstance()->getErrMsg(-2), seq);
		return 0;
	}

	return 0;
}

int GetMoneyProc::doResponse(CDLSocketHandler* /*clientHandler*/, InputPacket* /*inputPacket*/, Context* /*pt*/)
{
	return 0;
}

//REGISTER_PROCESS(CLIENT_MSG_GET_MONEY, GetMoneyProc);