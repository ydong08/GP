#include "PushOutCard.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_OUT_CARD, PushOutCard)

PushOutCard::PushOutCard()
{
	this->name = "PushOutCard";
}

PushOutCard::~PushOutCard()
{

} 

int PushOutCard::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if (player == NULL)
		return 0;

	//设置变量
	player->current_user_pos_ = INVALID_CHAIR;
	player->action_ = WIK_NULL;
	player->m_cbActionCard = 0;

	//删除牌
	player->game_logic_.RemoveCard(player->m_cbCardIndex[player->m_TableIndex], player->OperateCard_);

	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_OUT_CARD, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteInt(player->m_TableId);
	requestPacket.WriteByte(player->OperateCard_);

	requestPacket.End();
	this->send(clientHandler, &requestPacket);

	return 0;
}

int PushOutCard::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv SendCardProc Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if (retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();

	if (uid != player->m_Uid)
	{
		LOGGER(E_LOG_ERROR) << "PushOutCard doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}
	//变量定义
	WORD wMeChairID = player->m_TableIndex;

	//设置变量
	player->current_user_pos_ = INVALID_CHAIR;
	player->m_wOutCardUser = inputPacket->ReadInt();
	player->m_cbOutCardData = inputPacket->ReadByte();
	assert(player->m_cbOutCardData != 0);
	//删除扑克
	if (wMeChairID != player->m_wOutCardUser&&player->m_cbOutCardData!=0)
		player->game_logic_.RemoveCard(player->m_cbCardIndex[player->m_wOutCardUser], player->m_cbOutCardData);

	return 0;
}


