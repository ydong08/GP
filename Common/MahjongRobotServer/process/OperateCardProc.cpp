

#include "OperateCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>

//REGISTER_PROCESS(CLIENT_MSG_OPERATE_CARD, OperateCardProc)

OperateCardProc::OperateCardProc()
{
	this->name = "OperateCardProc";
}

OperateCardProc::~OperateCardProc()
{
}

int OperateCardProc::doRequest(CDLSocketHandler * client, InputPacket * inputPacket, Context * pt)
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if (player == NULL)
		return 0;

	//设置变量
	player->action_ = WIK_NULL;
	player->m_cbActionCard = 0;

	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_OPERATE_CARD, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteByte(player->OperateCode_);
	requestPacket.WriteByte(player->OperateCard_);

	requestPacket.End();
	this->send(clientHandler, &requestPacket);

	/*printf("Send ComingGameProc Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->name=[%s]\n", "robot");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->money=[%ld]\n", player->money);
	printf("Data Send: player->clevel=[%d]\n", player->clevel);*/
	return 0;
}

int OperateCardProc::doResponse(CDLSocketHandler * clientHandler, InputPacket * inputPacket, Context * pt)
{
	return 0;
}
