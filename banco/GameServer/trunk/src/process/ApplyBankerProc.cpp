#include "ApplyBankerProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "BaseClientHandler.h"
#include "json/json.h"
#include "ProcessManager.h"

ApplyBankerProc::ApplyBankerProc()
{
	this->name = "ApplyBankerProc";
}

ApplyBankerProc::~ApplyBankerProc()
{

} 

int ApplyBankerProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType() ;
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	short realTid = tid & 0x0000FFFF;
	short sid = pPacket->ReadShort();

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();
    Table *table = room->getTable(realTid);
	int i = 0;
	if(table==NULL)
	{
		_LOG_ERROR_("ApplyBankerProc:table is NULL\n");
		//sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(-2);
		response.WriteString(ErrorMsg::getInstance()->getErrMsg(-2));
		response.WriteInt(uid);
		response.WriteInt64(room->getTable(realTid)->m_nBankerlimit);
		response.WriteInt64(0);
		response.End();
		if(clientHandler)
			clientHandler->Send(response.packet_buf(), response.packet_size());
		return 0;
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("ApplyBankerProc: uid[%d] Player is NULL\n", uid);
		//sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(-1);
		response.WriteString(ErrorMsg::getInstance()->getErrMsg(-1));
		response.WriteInt(uid);
		response.WriteInt64(table->m_nBankerlimit);
		response.WriteInt64(0);
		response.End();
        if(clientHandler)
            clientHandler->Send(response.packet_buf(), response.packet_size());
		return 0;
	}

	if(player->m_lMoney < table->m_nBankerlimit)
	{
		_LOG_ERROR_("Your uid[%d] m_lMoney[%ld] is not enough high[%d]\n", uid, player->m_lMoney, table->m_nBankerlimit);
		//sendErrorMsg(hallhandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),seq);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(-4);
		response.WriteString(ErrorMsg::getInstance()->getErrMsg(-4));
		response.WriteInt(uid);
		response.WriteInt64(table->m_nBankerlimit);
		response.WriteInt64(player->m_lMoney);
		response.End();
		if(hallhandler)
			hallhandler->Send(&response,false);
		return 0;
	}

	if(table->isWaitForBanker(uid))
	{
		_LOG_ERROR_("You[%d] is the Banker, not necessary to apply\n", uid);
		//sendErrorMsg(hallhandler, cmd, uid, -5,ErrorMsg::getInstance()->getErrMsg(-5),seq);
		OutputPacket response;
		response.Begin(cmd,uid);
		response.SetSeqNum(seq);
		response.WriteShort(-5);
		response.WriteString(ErrorMsg::getInstance()->getErrMsg(-5));
		response.WriteInt(uid);
		response.WriteInt64(table->m_nBankerlimit);
		response.WriteInt64(player->m_lMoney);
		response.End();
		if(hallhandler)
			hallhandler->Send(&response,false);

		return 0;
	}
	
	player->setActiveTime(time(NULL));
	table->setToBankerList(player);

	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, player, seq);
		}
	}

	//说明现在没有人在做庄
	if(table->bankeruid == 0)
	{
		//马上让现在申请的用户上庄
		table->rotateBanker();
	}

	return 0;
}


int ApplyBankerProc::sendTabePlayersInfo(Player* player, Table* table, Player* applyer, short seq)
{
	OutputPacket response;
	response.Begin(CLIENT_MSG_APPLYBANKER, player->id);
	if(player->id == applyer->id)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(applyer->id);
	response.WriteInt64(table->m_nBankerlimit);
	response.WriteInt64(player->m_lMoney);
	response.WriteString(applyer->name);
	response.WriteShort(applyer->m_nSeatID);
	response.WriteString(applyer->json);
	response.End();
	//_LOG_DEBUG_("[Data Response] push to uid=[%d] m_nStatus=[%d]\n", player->id,player->m_nStatus);
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[ApplyBankerProc] Send To Uid[%d] Error!\n", player->id);
//	else
//		_LOG_DEBUG_("[ApplyBankerProc] Send To Uid[%d] Success\n", player->id);
	return 0;
}


REGISTER_PROCESS(CLIENT_MSG_APPLYBANKER, ApplyBankerProc)
