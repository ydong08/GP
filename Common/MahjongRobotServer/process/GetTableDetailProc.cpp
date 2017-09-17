#include "GetTableDetailProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetTableDetailProc)

GetTableDetailProc::GetTableDetailProc()
{
	this->name = "GetTableDetailProc";
}

GetTableDetailProc::~GetTableDetailProc()
{

} 

int GetTableDetailProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;

	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_TABLEDET, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteInt(player->m_TableId);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int GetTableDetailProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	
	if (retcode < 0 || player == NULL)
		return EXIT;
	
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	if ((uid != player->m_Uid) || (tid != player->m_TableId))
		return EXIT;

	int64_t cell_score = inputPacket->ReadInt64();
	player->banker_pos_ = inputPacket->ReadInt();
	player->current_user_pos_ = inputPacket->ReadInt();
	player->OperateCard_ = inputPacket->ReadByte();
	BYTE perform_action = inputPacket->ReadByte();
	player->left_card_count_ = inputPacket->ReadByte();

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		BYTE trustee = inputPacket->ReadByte();
	}

	player->m_wOutCardUser = (WORD)inputPacket->ReadInt();
	player->m_cbOutCardData = inputPacket->ReadByte();

	// ÆúÅÆÐÅÏ¢
	for (int i = 0; i < GAME_PLAYER; i++)
		player->m_cbDiscardCount[i] = inputPacket->ReadByte();
		
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		for (int j=0; j<player->m_cbDiscardCount[i]; j++)
			player->m_cbDiscardCard[i][j] = inputPacket->ReadByte();
	}

	// ÊÖÅÆ
	ZeroMemory(player->m_cbCardIndex, sizeof(player->m_cbCardIndex));
	for (int i=0; i<GAME_PLAYER; i++)
	{
		uint8_t card_count = inputPacket->ReadByte();
		for (int j = 0; j < MAX_COUNT; j++)
		{
			uint8_t card = inputPacket->ReadByte();
			if (0 != card)
			{
				uint8_t card_index = player->game_logic_.SwitchToCardIndex(card);
				player->m_cbCardIndex[i][card_index] += 1;
			}
		}
	}

	uint8_t cbSendCardData = inputPacket->ReadByte();

	for (int i = 0; i < GAME_PLAYER; i++)
		player->m_cbWeaveCount[i] = inputPacket->ReadByte();

	ZeroMemory(player->m_WeaveItemArray, sizeof(player->m_WeaveItemArray));
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		for (int j = 0; j < MAX_WEAVE; j++)
		{
			player->m_WeaveItemArray[i][j].cbWeaveKind = inputPacket->ReadByte();
			player->m_WeaveItemArray[i][j].cbCenterCard = inputPacket->ReadByte();
			player->m_WeaveItemArray[i][j].cbPublicCard = inputPacket->ReadByte();
			player->m_WeaveItemArray[i][j].wProvideUser = inputPacket->ReadInt();
		}
	}

	player->heap_head_ = (uint16_t)inputPacket->ReadInt();
	player->heap_tail_ = (uint16_t)inputPacket->ReadInt();

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		for (int j = 0; j < 2; j++)
			player->heap_info_[i][j] = inputPacket->ReadByte();
	}

	return 0;
}

