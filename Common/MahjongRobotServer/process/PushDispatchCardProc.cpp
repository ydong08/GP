#include "PushDispatchCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_DISPATCH_CARD, PushDispatchCardProc)

PushDispatchCardProc::PushDispatchCardProc()
{
	this->name = "PushDispatchCardProc";
}

PushDispatchCardProc::~PushDispatchCardProc()
{

} 

int PushDispatchCardProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int PushDispatchCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv PushGameStart Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if(retcode < 0 || player == NULL)
	{
		return EXIT;
	}

	int tempUid = inputPacket->ReadInt();

	//设置变量
	WORD wMeChairID = player->m_TableIndex;
	player->current_user_pos_ = inputPacket->ReadInt();
	int tail = inputPacket->ReadByte();
	int action_ = inputPacket->ReadByte();
	int card = inputPacket->ReadByte();
	int out_card_time = inputPacket->ReadInt();
	//丢弃扑克
	if ((player->m_wOutCardUser != INVALID_CHAIR) && (player->m_cbOutCardData != 0))
	{
		//丢弃扑克
		player->m_cbDiscardCard[player->m_wOutCardUser][player->m_cbDiscardCount[player->m_wOutCardUser]++] = player->m_cbOutCardData;
		//设置变量
		player->m_cbOutCardData = 0;
		player->m_wOutCardUser = INVALID_CHAIR;
	}

	//发牌处理
	if (card != 0)
	{
		player->m_cbCardIndex[player->current_user_pos_][player->game_logic_.SwitchToCardIndex(card)]++;
		//扣除扑克
		player->left_card_count_--;
	}

	//设置时间
	if (wMeChairID == player->current_user_pos_)
	{
		player->action_ = action_;
		player->m_cbActionCard = card;
		player->out_card_time_ = out_card_time;
		//计算时间
		player->startOutCardTimer(tempUid, 2 + rand() % 3);
	}
	return 0;
}


