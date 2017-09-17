#include "PushSendCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "Protocol.h"
#include <string>
using namespace std;

REGISTER_PROCESS(GMSERVER_MSG_SEND_CARD, PushSendCardProc)

PushSendCardProc::PushSendCardProc()
{
	this->name = "PushSendCardProc";
}

PushSendCardProc::~PushSendCardProc()
{

} 

int PushSendCardProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int PushSendCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
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
		LOGGER(E_LOG_ERROR) << "PushSendCardProc doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}
	player->reset_data();
	player->dice_point_ = inputPacket->ReadInt();
	player->banker_pos_ = inputPacket->ReadInt();
	player->current_user_pos_ = inputPacket->ReadInt();
	player->action_ = inputPacket->ReadByte();
	player->m_cbActionCard = 0;
	
	//出牌信息
	player->m_cbOutCardData = 0;
	player->m_wOutCardUser = INVALID_CHAIR;
	ZeroMemory(player->m_cbDiscardCard, sizeof(player->m_cbDiscardCard));
	ZeroMemory(player->m_cbDiscardCount, sizeof(player->m_cbDiscardCount));

	//组合扑克
	ZeroMemory(player->m_cbWeaveCount, sizeof(player->m_cbWeaveCount));
	ZeroMemory(player->m_WeaveItemArray, sizeof(player->m_WeaveItemArray));
	for (int i = 0; i < MAX_COUNT*GAME_PLAYER; i++)
	{
		BYTE card = inputPacket->ReadByte();
		player->m_Card.push_back(card);
	}
	player->heap_head_ = inputPacket->ReadInt();
	player->heap_tail_ = inputPacket->ReadInt();

	for (int k = 0; k < 4; k++)
	{
		for (int j = 0; j < 2; j++)
		{
			player->heap_info_[k][j] = inputPacket->ReadByte();	//堆立信息
		}
	}

	//设置扑克
	WORD wMeChairId = player->m_TableIndex;
	BYTE cbCardCount = (wMeChairId == player->banker_pos_) ? MAX_COUNT : (MAX_COUNT - 1);
	player->game_logic_.SwitchToCardIndex((BYTE*)player->m_Card.data(), cbCardCount, player->m_cbCardIndex[wMeChairId]);
	BYTE bIndex = 1;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == wMeChairId) continue;
		cbCardCount = (i == player->banker_pos_) ? MAX_COUNT : (MAX_COUNT - 1);
		player->game_logic_.SwitchToCardIndex((BYTE*)&player->m_Card[MAX_COUNT*bIndex++], cbCardCount, player->m_cbCardIndex[i]);
	}
	player->left_card_count_ = inputPacket->ReadByte();

	if (PacketBase::game_id != E_SICHUAN_MAHJONG_GAME_ID)
	{
		if ((player->action_ != WIK_NULL) || player->current_user_pos_ == wMeChairId)
		{
			player->startOutCardTimer(uid, 2 + rand() % 3);
		}
	}
	return 0;
}


